#include "stdafx.h"
#include "Controller.h"
#include "Target.h"
#include "IGUIDEDoc.h"

Controller::Controller()
{
	m_pThread = NULL;
	m_pGamePad = NULL;
	m_bRunning = false;
	m_bActive = false;

}

Controller::~Controller()
{
	delete m_pThread;
	delete m_pGamePad;
}

void Controller::reset(){

	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();

	if (pDoc->m_InputController == L"Gamepad") {
		if (!m_pGamePad)
		m_pGamePad = new DirectX::GamePad;
		m_bActive = true;
	}

	else {
		m_bActive = false;
	}

	if (m_bRunning) {
		shutdown();
	}

	if (pDoc->m_FlipVertical == 1) {
		flipSign = -1;
	}

	else {
		flipSign = 1;
	}
	
	if (m_bActive) {
		m_pThread = AfxBeginThread(GamePadThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL);
		m_pThread->m_bAutoDelete = false;
		m_bRunning = true;
		m_pThread->ResumeThread();
	}

}

void Controller::shutdown() {
	
	m_bRunning = false;
	if (m_pThread)
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);

}

UINT GamePadThread(LPVOID pParam) {

	Controller* parent = (Controller*)pParam;
	DirectX::GamePad::State state;

	while (parent->m_bRunning) {

		while (parent->m_bActive) {

			state = parent->m_pGamePad->GetState(0);

			if (state.IsConnected()) {

				if (state.IsAPressed()) {

					parent->state.pushed = false;
					parent->state.fireDown = true;

				}

				if (!state.buttons.a)

				{

					parent->state.fireUp = true;
					parent->state.fireDown = false;

				}

				if (state.IsDPadDownPressed()) {

					if (!parent->state.pushed) {
						parent->state.LY -= parent->flipSign * 2;
						parent->state.pushed = true;
						Sleep(100);
					}

					else
						parent->state.LY -= parent->flipSign * 2;

				}

				if (state.IsDPadUpPressed()) {
					if (!parent->state.pushed) {
						parent->state.LY += parent->flipSign * 2;
						parent->state.pushed = true;
						Sleep(100);
					}

					else
						parent->state.LY += parent->flipSign * 2;

				}

				if (state.IsDPadLeftPressed()) {
					if (!parent->state.pushed) {
						parent->state.LX -= 2;
						parent->state.pushed = true;
						Sleep(100);
					}

					else
						parent->state.LX -= 2;
				}

				if (state.IsDPadRightPressed()) {
					if (!parent->state.pushed) {
						parent->state.LX += 2;
						parent->state.pushed = true;
						Sleep(100);
					}

					else
						parent->state.LX += 2;
				}

				if (parent->state.fireUp && parent->state.fireDown) {
					parent->state.fired++;
					parent->state.fireDown = false;
				}

			}

			Sleep(50);

			if (parent->m_pThread)
			PostMessage(AfxGetMainWnd()->GetSafeHwnd(), AFX_WM_DRAW2D, 0, 0);

		}

	}

	return 0;
}
