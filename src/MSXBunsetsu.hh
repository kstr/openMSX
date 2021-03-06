#ifndef MSXBUNSETSU_HH
#define MSXBUNSETSU_HH

#include "MSXDevice.hh"
#include "Rom.hh"

namespace openmsx {

class MSXBunsetsu final : public MSXDevice
{
public:
	explicit MSXBunsetsu(const DeviceConfig& DeviceConfig);

	void reset(EmuTime::param time) override;

	byte readMem(word address, EmuTime::param time) override;
	void writeMem(word address, byte value, EmuTime::param time) override;
	const byte* getReadCacheLine(word start) const override;
	byte* getWriteCacheLine(word start) const override;

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	Rom bunsetsuRom;
	Rom jisyoRom;
	unsigned jisyoAddress;
};

} // namespace openmsx

#endif
