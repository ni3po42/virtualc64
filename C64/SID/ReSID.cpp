/*
 * (C) 2011 - 2017 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

ReSID::ReSID()
{
	setDescription("ReSID");
	debug(3, "  Creating ReSID at address %p...\n", this);

    sid = new reSID::SID();
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Configuration items
        { &sampleRate,          sizeof(sampleRate),             KEEP_ON_RESET },
        { &emulateFilter,       sizeof(emulateFilter),          KEEP_ON_RESET },
        
        // ReSID state
        { st.sid_register,                  sizeof(st.sid_register),                  KEEP_ON_RESET },
        { &st.bus_value,                    sizeof(st.bus_value),                     KEEP_ON_RESET },
        { &st.bus_value_ttl,                sizeof(st.bus_value_ttl),                 KEEP_ON_RESET },
        { &st.write_pipeline,               sizeof(st.write_pipeline),                KEEP_ON_RESET },
        { &st.write_address,                sizeof(st.write_address),                 KEEP_ON_RESET },
        { &st.voice_mask,                   sizeof(st.voice_mask),                    KEEP_ON_RESET },
        { &st.accumulator[0],               sizeof(st.accumulator[0]),                KEEP_ON_RESET },
        { &st.accumulator[1],               sizeof(st.accumulator[1]),                KEEP_ON_RESET },
        { &st.accumulator[2],               sizeof(st.accumulator[2]),                KEEP_ON_RESET },
        { &st.shift_register[0],            sizeof(st.shift_register[0]),             KEEP_ON_RESET },
        { &st.shift_register[1],            sizeof(st.shift_register[1]),             KEEP_ON_RESET },
        { &st.shift_register[2],            sizeof(st.shift_register[2]),             KEEP_ON_RESET },
        { &st.shift_register_reset[0],      sizeof(st.shift_register_reset[0]),       KEEP_ON_RESET },
        { &st.shift_register_reset[1],      sizeof(st.shift_register_reset[1]),       KEEP_ON_RESET },
        { &st.shift_register_reset[2],      sizeof(st.shift_register_reset[2]),       KEEP_ON_RESET },
        { &st.shift_pipeline[0],            sizeof(st.shift_pipeline[0]),             KEEP_ON_RESET },
        { &st.shift_pipeline[1],            sizeof(st.shift_pipeline[1]),             KEEP_ON_RESET },
        { &st.shift_pipeline[2],            sizeof(st.shift_pipeline[2]),             KEEP_ON_RESET },
        { &st.pulse_output[0],              sizeof(st.pulse_output[0]),               KEEP_ON_RESET },
        { &st.pulse_output[1],              sizeof(st.pulse_output[1]),               KEEP_ON_RESET },
        { &st.pulse_output[2],              sizeof(st.pulse_output[2]),               KEEP_ON_RESET },
        { &st.floating_output_ttl[0],       sizeof(st.floating_output_ttl[0]),        KEEP_ON_RESET },
        { &st.floating_output_ttl[1],       sizeof(st.floating_output_ttl[1]),        KEEP_ON_RESET },
        { &st.floating_output_ttl[2],       sizeof(st.floating_output_ttl[2]),        KEEP_ON_RESET },
        { &st.rate_counter[0],              sizeof(st.rate_counter[0]),               KEEP_ON_RESET },
        { &st.rate_counter[1],              sizeof(st.rate_counter[1]),               KEEP_ON_RESET },
        { &st.rate_counter[2],              sizeof(st.rate_counter[2]),               KEEP_ON_RESET },
        { &st.rate_counter_period[0],       sizeof(st.rate_counter_period[0]),        KEEP_ON_RESET },
        { &st.rate_counter_period[1],       sizeof(st.rate_counter_period[1]),        KEEP_ON_RESET },
        { &st.rate_counter_period[2],       sizeof(st.rate_counter_period[2]),        KEEP_ON_RESET },
        { &st.exponential_counter[0],       sizeof(st.exponential_counter[0]),        KEEP_ON_RESET },
        { &st.exponential_counter[1],       sizeof(st.exponential_counter[1]),        KEEP_ON_RESET },
        { &st.exponential_counter[2],       sizeof(st.exponential_counter[2]),        KEEP_ON_RESET },
        { &st.exponential_counter_period[0],sizeof(st.exponential_counter_period[0]), KEEP_ON_RESET },
        { &st.exponential_counter_period[1],sizeof(st.exponential_counter_period[1]), KEEP_ON_RESET },
        { &st.exponential_counter_period[2],sizeof(st.exponential_counter_period[2]), KEEP_ON_RESET },
        { &st.envelope_counter[0],          sizeof(st.envelope_counter[0]),           KEEP_ON_RESET },
        { &st.envelope_counter[1],          sizeof(st.envelope_counter[1]),           KEEP_ON_RESET },
        { &st.envelope_counter[2],          sizeof(st.envelope_counter[2]),           KEEP_ON_RESET },
        { &st.envelope_state[0],            sizeof(st.envelope_state[0]),             KEEP_ON_RESET },
        { &st.envelope_state[1],            sizeof(st.envelope_state[1]),             KEEP_ON_RESET },
        { &st.envelope_state[2],            sizeof(st.envelope_state[2]),             KEEP_ON_RESET },
        { &st.hold_zero[0],                 sizeof(st.hold_zero[0]),                  KEEP_ON_RESET },
        { &st.hold_zero[1],                 sizeof(st.hold_zero[1]),                  KEEP_ON_RESET },
        { &st.hold_zero[2],                 sizeof(st.hold_zero[2]),                  KEEP_ON_RESET },
        { &st.envelope_pipeline[0],         sizeof(st.envelope_pipeline[0]),          KEEP_ON_RESET },
        { &st.envelope_pipeline[1],         sizeof(st.envelope_pipeline[1]),          KEEP_ON_RESET },
        { &st.envelope_pipeline[2],         sizeof(st.envelope_pipeline[2]),          KEEP_ON_RESET },
        
        { NULL,                             0,                                        0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    // Set default values
    sid->set_chip_model(reSID::MOS6581);
    sampleRate = 44100;
    sid->set_sampling_parameters((double)PAL_CYCLES_PER_FRAME * PAL_REFRESH_RATE,
                                 reSID::SAMPLE_FAST,
                                 (double)sampleRate);
    
    setAudioFilter(true);
}

ReSID::~ReSID()
{
    delete sid;
}

void
ReSID::reset()
{
    debug("ReSID::reset\n");
    VirtualComponent::reset();
    sid->reset();
}

void
ReSID::setChipModel(SIDChipModel model)
{
    sid->set_chip_model((reSID::chip_model)model);
    
    debug("Emulating SID model %s.\n",
          (model == reSID::MOS6581) ? "MOS6581" :
          (model == reSID::MOS8580) ? "MOS8580" : "?");
}

void
ReSID::setClockFrequency(uint32_t value)
{
    double frequency = (double)value;
    reSID::sampling_method method = sid->sampling;
    double rate = (double)sampleRate;
    
    sid->set_sampling_parameters(frequency, method, rate);
    
    debug("Changing clock frequency to %d\n", value);
}

void
ReSID::setSampleRate(uint32_t value)
{
    double frequency = sid->clock_frequency;
    reSID::sampling_method method = sid->sampling;
    double rate = (double)value;
    
    sid->set_sampling_parameters(frequency, method, rate);
    
    debug("Changing sample rate to %d\n", value);
}

void 
ReSID::setAudioFilter(bool value)
{
    emulateFilter = value;
    sid->enable_filter(value);
    
    debug("%s audio filter emulation.\n", value ? "Enabling" : "Disabling");
}

void 
ReSID::setSamplingMethod(SamplingMethod value)
{
    double frequency = sid->clock_frequency;
    reSID::sampling_method method = (reSID::sampling_method)value;
    double rate = (double)sampleRate;
    
    sid->set_sampling_parameters(frequency, method, rate);
    
    debug("Changing ReSID sampling method to %s.\n",
          (method == reSID::SAMPLE_FAST) ? "SAMPLE_FAST" :
          (method == reSID::SAMPLE_INTERPOLATE) ? "SAMPLE_INTERPOLATE" :
          (method == reSID::SAMPLE_RESAMPLE) ? "SAMPLE_RESAMPLE" :
          (method == reSID::SAMPLE_RESAMPLE_FASTMEM) ? "SAMPLE_RESAMPLE_FASTMEM" : "?");
}

void
ReSID::loadFromBuffer(uint8_t **buffer)
{
    VirtualComponent::loadFromBuffer(buffer);
    sid->write_state(st);
}

void
ReSID::saveToBuffer(uint8_t **buffer)
{
    st = sid->read_state();
    VirtualComponent::saveToBuffer(buffer);
}

uint8_t
ReSID::peek(uint16_t addr)
{	
    return sid->read(addr);
}

void 
ReSID::poke(uint16_t addr, uint8_t value)
{
    sid->write(addr, value);
}

void
ReSID::execute(uint64_t elapsedCycles)
{
    short buf[2049];
    int buflength = 2048;
    
    if (elapsedCycles > PAL_CYCLES_PER_SECOND) {
        warn("Number of missing SID cycles is far too large.\n");
        elapsedCycles = PAL_CYCLES_PER_SECOND;
    }

    reSID::cycle_count delta_t = (reSID::cycle_count)elapsedCycles;
    int bufindex = 0;
    
    // Let reSID compute some sound samples
    while (delta_t) {
        bufindex += sid->clock(delta_t, buf + bufindex, buflength - bufindex);
    }
    
    // Write samples into ringbuffer
    if (bufindex) {
        bridge->writeData(buf, bufindex);
    }
}

void
ReSID::dumpState()
{
	msg("ReSID\n");
	msg("-----\n\n");
    msg("   Sample rate : %d\n", getSampleRate());
    msg(" CPU frequency : %d\n", getClockFrequency());
	msg("\n");
}

SIDInfo
ReSID::getInfo()
{
    SIDInfo info;
    reSID::SID::State state = sid->read_state();

    for (unsigned i = 0; i < 3; i++) {
        uint8_t *sidreg = (uint8_t *)state.sid_register + (i * 7);
        VoiceInfo *vinfo = (i == 0) ? &info.voice1 : (i == 1) ? &info.voice2 : &info.voice3;
        vinfo->frequency = HI_LO(sidreg[0x01], sidreg[0x00]);
        vinfo->pulseWidth = ((sidreg[3] & 0x0F) << 8) | sidreg[0x02];
        vinfo->waveform = sidreg[0x04] & 0xF0;
        vinfo->ringMod = (sidreg[0x04] & 0x04) != 0;
        vinfo->hardSync = (sidreg[0x04] & 0x02) != 0;
        vinfo->gateBit = (sidreg[0x04] & 0x01) != 0;
        vinfo->testBit = (sidreg[0x04] & 0x08) != 0;
        vinfo->attackRate = sidreg[0x05] >> 4;
        vinfo->decayRate = sidreg[0x05] & 0x0F;
        vinfo->sustainRate = sidreg[0x06] >> 4;
        vinfo->releaseRate = sidreg[0x06] & 0x0F;
        vinfo->filterOn = GET_BIT(state.sid_register[0x17], i) != 0;
    }
    info.volume = state.sid_register[0x18] & 0x0F;
    info.filterType = state.sid_register[0x18] & 0x70;
    info.filterCutoff = (state.sid_register[0x16] << 3) | (state.sid_register[0x15] & 0x07);
    
    return info;
}


