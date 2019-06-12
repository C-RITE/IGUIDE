#pragma once

// wrapper class for Gamepad Controller

struct ControlState {
	ControlState() : LX(0),LY(0),pushed(false),fireDown(false),fireUp(true),fired(0) {};
	int LX;					// DPAD x pos
	int LY;					// DPAD y pos
	bool pushed;			// for input delay
	bool fireDown;			// for fire signal state
	bool fireUp;			// for fire signal state
	int fired;				// counter for A being fired
	bool operator==(const ControlState &a)
	{
		if (memcmp(this,&a,sizeof(ControlState)) == 0)
			return true;
		else
			return false;
	}
};

UINT GamePadThread(LPVOID pParam);

class Controller
{

public:
	Controller();
	~Controller();

	CWinThread*			m_pThread;

	void				reset();
	void				shutdown();
	ControlState		state;								// store controller states
	int					flipSign;
	bool				m_bRunning;							// thread runtime
	bool				m_bActive;							// gamepad on/off
};

