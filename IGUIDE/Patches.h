#pragma once
#include <vector>

struct Patch {

	CD2DPointF		coordsDEG;
	CD2DPointF		coordsPX;
	D2D1_COLOR_F	color;
	CString			timestamp;
	CString			vidfilename;
	double			rastersize;
	bool			locked;
	bool			visited;
	int				index;
	int				region;
	CString			wavelength;
	CString			vidlength;
	CString			defocus;

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
	CString				filename;
	
	bool				SaveToFile(CString directory);
	bool				isFileTouched() { return fileTouched; };
	void				commit(Patches::iterator &patch);
	int					getProgress(int region, int &size);
	void				untouch();
	void				setOverlap(float overlap, float rsDeg);	
	void				setSubject(CString prefix) { subject = prefix; };
	void				duplicate(Patches::iterator &current);


private:
	
	CD2DSizeF		overlap;
	CString			subject;
	CString			timestamp;
	int				index;	
	bool			fileTouched;
	void			GetSysTime(CString &buf);

	static	DWORD WINAPI	ThreadWaitDigest(LPVOID pParam);	// if connected to ICANDI, wait for digested response

};