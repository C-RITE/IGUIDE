#include "stdafx.h"
#include "Controller.h"
#include "Target.h"
#include "IGUIDEDoc.h"
#include "GamePad.h"

Controller::Controller()
{

	m_pThread = NULL;
	m_bRunning = false;
	m_bActive = false;
	state.fired = -1;

}

Controller::~Controller()
{
}

void Controller::reset(){

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	if (pDoc->m_InputController == L"Gamepad") {
		m_bActive = true;
	}

	else {
		m_bActive = false;
	}

	flipSign = pDoc->m_FlipVertical;
	
	if (m_bActive && m_pThread == NULL) {
		m_pThread = AfxBeginThread(GamePadThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
		m_pThread->m_bAutoDelete = false;
		m_bRunning = true;
		m_pThread->ResumeThread();
	}

}

void Controller::shutdown() {
	
	m_bActive = false;
	m_bRunning = false;
	if (m_pThread) {
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		delete m_pThread;
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

				if (state.IsAPressed())
					parent->state.fired++;

				if (state.IsDPadDownPressed()) {
					parent->state.LY += (parent->flipSign * 1);
				}

				if (state.IsDPadUpPressed()) {
					(parent->state.LY -= parent->flipSign * 1);
				}

				if (state.IsDPadLeftPressed())
					parent->state.LX -= 1;

				if (state.IsDPadRightPressed()) 
					parent->state.LX += 1;

			}

			if (parent->state == localState) 
				continue;

			while (state.buttons.a){
				Sleep(10);
				state = m_pGamePad->GetState(0);
			}

			if (parent->state.fired > localState.fired) 
				PostMessage(mainWnd, GAMEPAD_UPDATE, 1, 0);	// we hit the fire button!
			else {
				PostMessage(mainWnd, GAMEPAD_UPDATE, 0, 0); // we just moved around.
				Sleep(100);
			}

		}

	}

	delete m_pGamePad;

	return 0;

}
