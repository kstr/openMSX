// $Id$

/**
 * Based on:
 *    Z80Em: Portable Z80 emulator
 *    written by Marcel de Kogel 1996,1997
 * heavily rewritten to fit openMSX structure
 */

#ifndef __Z80_HH__
#define __Z80_HH__

#include "config.h"
#include "openmsx.hh"
#include "CPU.hh"
#include "EmuTime.hh"

#undef  _CPU_
#define _CPU_ Z80
#undef  ResumeFunc
#define ResumeFunc Z80_ResumeFunc

class CPUInterface;
class Z80;
typedef void (Z80::*Z80_ResumeFunc)();


class Z80 : public CPU {
	public:
		Z80(CPUInterface *interf, const EmuTime &time);
		virtual ~Z80();
		virtual void setCurrentTime(const EmuTime &time);
		virtual const EmuTime &getCurrentTime() const;

	private:
		#include "CPUCore.n1"
		
		static const int IO_DELAY1 = 1;
		static const int IO_DELAY2 = 3;
		static const int MEM_DELAY1 = 1;
		static const int MEM_DELAY2 = 2;
		static const int WAIT_CYCLES = 1;
		
		EmuTimeFreq<3579545> currentTime;

		// opcode function pointers
		static const Z80_ResumeFunc opcode_dd_cb[256];
		static const Z80_ResumeFunc opcode_fd_cb[256];
		static const Z80_ResumeFunc opcode_cb[256];
		static const Z80_ResumeFunc opcode_dd[256];
		static const Z80_ResumeFunc opcode_ed[256];
		static const Z80_ResumeFunc opcode_fd[256];
		static const Z80_ResumeFunc opcode_main[256];

		Z80_ResumeFunc resume;
};

#endif

