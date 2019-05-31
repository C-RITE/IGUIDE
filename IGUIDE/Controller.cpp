#include "stdafx.h"
#include "Controller.h"
#include "IGUIDEDoc.h"

Controller::Controller()
{
}

Controller::~Controller()
{
}

void Controller::init(Target* pTarget) {
	m_pTarget = pTarget;
	m_pGamePad = new DirectX::GamePad();
	m_pThread = AfxBeginThread(GamePadThread, this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, 0);
	m_pThread->m_bAutoDelete = false;
	m_bRunning = true;
	m_pThread->ResumeThread();


}

void Controller::reset(){

	int flipSign;
	CIGUIDEDoc* pDoc = CIGUIDEDoc::GetDoc();
	if (pDoc->m_FlipVertical == 1) {
		flipSign = -1;
	}
	else {
		flipSign = 1;
	}

}

void Controller::shutdown() {
	
	m_bRunning = false;
	WaitForSingleObject(m_pThread->m_hThread, INFINITE);
	delete m_pThread;
	delete m_pGamePad;
}

UINT GamePadThread(LPVOID pParam) {

	Controller* parent = (Controller*)pParam;
	DirectX::GamePad::State state;

	while (parent->m_bRunning) {
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

			if (parent->m_pTarget)
				parent->m_pTarget->Invalidate();
		}

		Sleep(50);
	}

	return 0;

}
