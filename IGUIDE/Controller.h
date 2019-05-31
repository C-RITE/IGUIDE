#pragma once
#include "GamePad.h"

class Target;
class CIGUIDEDoc;

// wrapper class for XboxController

struct XboxControlState {
	XboxControlState() : LX(0),LY(0),pushed(false),fireDown(false),fireUp(false),fired(0) {};
	int LX;					// DPAD x pos
	int LY;					// DPAD y pos
	bool pushed;			// for input delay
	bool fireDown;			// for fire signal state
	bool fireUp;			// for fire signal state
	int fired;				// counter for A being fired
};

UINT GamePadThread(LPVOID pParam);

class Controller 
{

public:
	Controller();
	~Controller();

	DirectX::GamePad*	m_pGamePad;
	Target*				m_pTarget;		// where subject calibration takes place
	CWinThread*			m_pThread;

	void				reset();
	void				init(Target* pTarget);
	void				shutdown();
	XboxControlState	state;								// store controller states
	XboxControlState	getState() { return this->state; };
	int					flipSign;
	bool				m_bRunning;
};

