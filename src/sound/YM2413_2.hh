// $Id$

#ifndef YM2413_2_HH
#define YM2413_2_HH

#include "YM2413Core.hh"
#include "SoundDevice.hh"
#include "Resample.hh"
#include "FixedPoint.hh"
#include "openmsx.hh"
#include <memory>

namespace openmsx {

class EmuTime;
class MSXMotherBoard;
class YM2413_2Debuggable;

/** 16.16 fixed point type for frequency calculations.
  */
typedef FixedPoint<16> FreqIndex;

/** 8.24 fixed point type for LFO calculations.
  */
typedef FixedPoint<24> LFOIndex;

class Globals;
class Channel;

class Slot
{
public:
	Slot();

	/**
	 * Initializes those parts that cannot be initialized in the constructor,
	 * because the constructor cannot have arguments since we want to create
	 * an array of Slots.
	 * This method should be called once, as soon as possible after
	 * construction.
	 */
	void init(Globals& globals, Channel& channel);

	/**
	 * Update phase increment counter of operator.
	 * Also updates the EG rates if necessary.
	 */
	void updateGenerators();

	inline int op_calc(FreqIndex phase, FreqIndex pm);
	inline void updateModulator();
	inline void KEY_ON (byte key_set);
	inline void KEY_OFF(byte key_clr);

	/**
	 * Output of SLOT 1 can be used to phase modulate SLOT 2.
	 */
	inline FreqIndex getPhaseModulation() {
		return FreqIndex(op1_out[0] << 1);
	}

	/**
	 * Sets the frequency multiplier [0..15].
	 */
	inline void setFrequencyMultiplier(byte value);

	/**
	 * Sets the key scale rate: true->0, false->2.
	 */
	inline void setKeyScaleRate(bool value);

	/**
	 * Sets the envelope type: true->sustained, false->percussive.
	 */
	inline void setEnvelopeType(bool value);

	/**
	 * Enables (true) or disables (false) vibrato.
	 */
	inline void setVibrato(bool value);

	/**
	 * Enables (true) or disables (false) amplitude modulation.
	 */
	inline void setAmplitudeModulation(bool value);

	/**
	 * Sets the total level: [0..63].
	 */
	inline void setTotalLevel(byte value);

	/**
	 * Sets the key scale level: 0->0 / 1->1.5 / 2->3.0 / 3->6.0 dB/OCT.
	 */
	inline void setKeyScaleLevel(byte value);

	/**
	 * Sets the waveform: 0 = sinus, 1 = half sinus, half silence.
	 */
	inline void setWaveform(byte value);

	/**
	 * Sets the amount of feedback [0..7].
	 */
	inline void setFeedbackShift(byte value);

	/**
	 * Sets the attack rate [0..15].
	 */
	inline void setAttackRate(byte value);

	/**
	 * Sets the decay rate [0..15].
	 */
	inline void setDecayRate(byte value);

	/**
	 * Sets the sustain level [0..15].
	 */
	inline void setSustainLevel(byte value);

	/**
	 * Sets the release rate [0..15].
	 */
	inline void setReleaseRate(byte value);

	// Phase Generator
	FreqIndex phase;	// frequency counter
	FreqIndex freq;	// frequency counter step

	int wavetable;	// waveform select

	// Envelope Generator
	int TL;		// total level: TL << 2
	int TLL;	// adjusted now TL
	int volume;	// envelope counter
	int sl;		// sustain level: sl_tab[SL]
	byte eg_type;	// percussive/nonpercussive mode
	byte state;	// phase type

	byte eg_sh_dp;	// (dump state)
	byte eg_sel_dp;	// (dump state)
	byte eg_sh_ar;	// (attack state)
	byte eg_sel_ar;	// (attack state)
	byte eg_sh_dr;	// (decay state)
	byte eg_sel_dr;	// (decay state)
	byte eg_sh_rr;	// (release state for non-perc.)
	byte eg_sel_rr;	// (release state for non-perc.)
	byte eg_sh_rs;	// (release state for perc.mode)
	byte eg_sel_rs;	// (release state for perc.mode)

	byte ar;	// attack rate: AR<<2
	byte dr;	// decay rate:  DR<<2
	byte rr;	// release rate:RR<<2
	byte KSR;	// key scale rate
	byte ksl;	// keyscale level
	byte kcodeScaled;	// key scale rate: kcode>>KSR
	byte mul;	// multiple: mul_tab[ML]

	// LFO
	byte AMmask;	// LFO Amplitude Modulation enable mask
	byte vib;	// LFO Phase Modulation enable flag (active high)

private:
	Globals* globals;
	Channel* channel;

	int op1_out[2];	// slot1 output for feedback
	byte fb_shift;	// feedback shift value

	byte key;	// 0 = KEY OFF, >0 = KEY ON
};

class Channel
{
public:
	Channel();
	inline int chan_calc();

	/**
	 * Initializes those parts that cannot be initialized in the constructor,
	 * because the constructor cannot have arguments since we want to create
	 * an array of Channels.
	 * This method should be called once, as soon as possible after
	 * construction.
	 */
	void init(Globals& globals);

	/**
	 * Sets some synthesis parameters as specified by the instrument.
	 * @param inst Pointer to instrument data.
	 * @param part Part [0..7] of the instrument.
	 */
	void setInstrumentPart(int instrument, int part);

	/**
	 * Sets all synthesis parameters as specified by the instrument.
	 * @param inst Pointer to instrument data.
	 */
	void setInstrument(int instrument);

	/**
	 * Sets all synthesis parameters as specified by the current instrument.
	 * The current instrument is determined by instvol_r.
	 */
	inline void setInstrument() {
		setInstrument(instvol_r >> 4);
	}

	Slot slots[2];

	/**
	 * Instrument/volume (or volume/volume in rhythm mode).
	 */
	byte instvol_r;

	// phase generator state
	int block_fnum;	// block+fnum
	FreqIndex fc;	// Freq. freqement base
	int ksl_base;	// KeyScaleLevel Base step
	byte kcode;	// key code (for key scaling)
	byte sus;	// sus on/off (release speed in percussive mode)

private:
	Globals* globals;
};

class Globals
{
public:
	Globals();

	// instrument settings
	//   0     - user instrument
	//   1-15  - fixed instruments
	//   16    - bass drum settings
	//   17-18 - other percussion instruments
	byte inst_tab[19][8];

	byte LFO_AM;
	byte LFO_PM;
};

class YM2413_2 : public YM2413Core, public SoundDevice, private Resample<1>
{
public:
	YM2413_2(MSXMotherBoard& motherBoard, const std::string& name,
	         const XMLElement& config, const EmuTime& time);
	virtual ~YM2413_2();

	void reset(const EmuTime& time);
	void writeReg(byte r, byte v, const EmuTime& time);

private:
	static inline FreqIndex fnumToIncrement(int fnum);

	void checkMute();
	bool checkMuteHelper();

	void init_tables();

	inline void advance_lfo();
	inline void advance();
	inline FreqIndex genPhaseHighHat();
	inline FreqIndex genPhaseSnare();
	inline FreqIndex genPhaseCymbal();
	inline void rhythm_calc(int** bufs, unsigned sample);
	inline int adjust(int x);

	/**
	 * Called when the custom instrument (instrument 0) has changed.
	 * @param part Part [0..7] of the instrument.
	 */
	void updateCustomInstrument(int part);

	void setRhythmMode(bool newMode);

	// SoundDevice
	virtual void setVolume(int newVolume);
	virtual void setOutputRate(unsigned sampleRate);
	virtual void generateChannels(int** bufs, unsigned num);
	virtual void updateBuffer(unsigned length, int* buffer,
		const EmuTime& time, const EmuDuration& sampDur);

	// Resample
	virtual void generateInput(float* buffer, unsigned num);

	friend class YM2413_2Debuggable;
	const std::auto_ptr<YM2413_2Debuggable> debuggable;

	int maxVolume;

	Globals globals;

	Channel channels[9];	// OPLL chips have 9 channels

	unsigned eg_cnt;	// global envelope generator counter

	// LFO
	LFOIndex lfo_am_cnt;
	LFOIndex lfo_pm_cnt;

	int noise_rng;		// 23 bit noise shift register

	byte reg[0x40];

	bool rhythm;		// Rhythm mode
};

} // namespace openmsx

#endif
