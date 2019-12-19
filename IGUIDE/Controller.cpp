#include "stdafx.h"
#include "Controller.h"
#include "Target.h"
#include "IGUIDEDoc.h"
#include "IGUIDE.h"
#include "GamePad.h"

Controller::Controller()
{

	m_pThread = NULL;
	m_bRunning = false;
	m_bActive = false;
	state.fired = -1;
	state.accel = 100;

}

Controller::~Controller()
{
}

void Controller::reset(){

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	if (pDoc->m_InputController == L"Gamepad") {
		setFlip();
		m_bActive = true;
	}
	
	else {
		m_bActive = false;
		m_bRunning = false;
	}


	if (m_bActive) {
		m_pThread = AfxBeginThread(GamePadThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
		m_bRunning = true;
		m_pThread->ResumeThread();
	}

}


void Controller::setFlip() {
	
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	if (pDoc->m_FlipVertical == L"True")
		flipSign.y = -1;
	else
		flipSign.y = 1;

	if (pDoc->m_FlipHorizontal == L"True")
		flipSign.x = -1;
	else
		flipSign.x = 1;

}

void Controller::shutdown() {
	
	m_bActive = false;
	m_bRunning = false;
	if (m_pThread) {
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
	}

}

UINT GamePadThread(LPVOID pParam) {

	Controller* parent = (Controller*)pParam;
	DirectX::GamePad* m_pGamePad; 
	m_pGamePad = new DirectX::GamePad;
	DirectX::GamePad::State state;
	ControlState localState;
	
	HWND mainWnd = AfxGetMainWnd()->GetSafeHwnd();

	while (parent->m_bRunning) {

		while (parent->m_bActive) {

			localState = parent->state;
			state = m_pGamePad->GetState(0);

			if (state.IsConnected()) {

				if (state.IsAPressed()) {
					parent->state.fired++;
					PostMessage(mainWnd, GAMEPAD_UPDATE, 1, 0);	// we hit the fire button!
					while (state.IsAPressed()) {
						Sleep(10);
						state = m_pGamePad->GetState(0);
					}
				}

				while (state.IsDPadDownPressed() || state.IsDPadLeftPressed() || state.IsDPadRightPressed() || state.IsDPadUpPressed()) {
					if (parent->state.accel > 5) 
						parent->state.accel -= 5;

					if (state.IsDPadDownPressed())
						parent->state.LY += parent->flipSign.y;

					if (state.IsDPadUpPressed())
						parent->state.LY -= parent->flipSign.y;

					if (state.IsDPadLeftPressed())
						parent->state.LX -= parent->flipSign.x;

					if (state.IsDPadRightPressed())
						parent->state.LX += parent->flipSign.x;

					PostMessage(mainWnd, GAMEPAD_UPDATE, 0, 0); // we just moved around...

					Sleep(parent->state.accel);
					
					state = m_pGamePad->GetState(0);

				}


			}

			else if (parent->m_bActive){

				CIGUIDEApp* IGUIDE = (CIGUIDEApp*)AfxGetApp();
				int answer = IGUIDE->m_pMainWnd->MessageBox(L"Gamepad not found! Please check connection.\nFall back to mouse?", L"Attention", MB_ICONHAND | MB_YESNOCANCEL);

				if (answer == IDYES) {

					parent->m_bActive = false;
					parent->m_bRunning = false;

					PostMessage(mainWnd, MOUSE_FALLBACK, 0, 0);

				}

				if (answer == IDCANCEL) {

					parent->m_bActive = false;
					parent->m_bRunning = false;
					
				}

			}

			// controller back to initial state
			// reset accellerator
			if (parent->state == localState && parent->state.accel < 100) {
				parent->state.accel = 100;
			}


		}

	}

	delete m_pGamePad;

	return 0;

}
