#pragma once
#include <vector>

struct Patch {

	CD2DPointF			coordsDEG;
	CD2DPointF			coordsPX;
	D2D1_COLOR_F		color;
	CString				timestamp;
	CString				vidfilename;
	double				rastersize;
	bool				locked;
	UINT				uID;
	int					index;
	int					region;
	CString				wavelength;
	CString				vidlength;
	CString				vidnumber;
	CString				defocus;

	bool operator==(Patch right) const {
		return true;
	}

};

enum Element
{
	NEXT = 1,
	PREV = 2
};

class Patches : public std::vector<Patch>
{

public:

	Patches();

	void				commit(Patches::iterator patch);
	int					getProgress(int region, int &size);
	void				setOverlap(float overlap, float rsDeg);	
	void				makePatchMatrix(SIZE wheel, CPoint pos, CD2DPointF posDeg, float rsDeg);
	Patches::iterator	add(Patch p);

private:
	
	CD2DSizeF			overlap;
	CString				timestamp;
	int					index;
	int					uID;

	static	DWORD WINAPI	ThreadWaitDigest(LPVOID pParam);	// if connected to ICANDI, wait for digested response

};