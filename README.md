## Synthesizer

### Team Members:
torre324, 

### Project Objectives

<ul>
    <li>An ability to use an external DAC to implement sinusodal waveforms for the oscillator </li>
    <li>An ability to configure GPIO pins to read ADC input in order to adjust the parameters for the FX chain</li>
    <li>An ability to use PIO peripheral to implement the I2S protocol for communicating with the DAC
    <li>An ability to use UART in order to read midi data from a midi controller </li>
</ul>

### Description

The goal of this project is to make a single oscillator synthesizer. The midi controller will send midi information (keynote status, keynote, velocity) over UART to the proton board which will generate a waveform for the corresponding frequency of the note played. Using ADC pins, the user will have the option to adjust potentiometers in order to control the paramters of the FXs. The final processed wave will go through a DAC (PCM5102A) in order to output the analog audio signal.

### Parts

[Midi Controller](https://www.amazon.com/Donner-N-25-Controlle-Velocity-Sensitive-Production/dp/B0C1ZC45F5/?_encoding=UTF8&pd_rd_w=Grd2g&content-id=amzn1.sym.4efc43db-939e-4a80-abaf-50c6a6b8c631%3Aamzn1.symc.5a16118f-86f0-44cd-8e3e-6c5f82df43d0&pf_rd_p=4efc43db-939e-4a80-abaf-50c6a6b8c631&pf_rd_r=B1KF2GA14WK6RNKPRT24&pd_rd_wg=U7XjY&pd_rd_r=86d487fc-a9b1-4629-b7ee-289ac83ccf03&ref_=pd_hp_d_atf_ci_mcx_mr_ca_hp_atf_d&th=1)<br />
[DAC PCM5102A](https://www.amazon.com/dp/B09C5QX228?ref=ppx_yo2ov_dt_b_fed_asin_title)<br />
10K potentiometers - In lab Kit <br />
Buttons - In Lab Kit


### Related Works

Title: Implmenting I2S with PIO <br />
URL: [URL](https://github.com/raspberrypi/pico-extras/tree/master/src/rp2_common/pico_audio_i2s) <br />
Differences: This is just one aspect that will be using in order to implement our project. We will likley need to adjust the output bit rate using a clock divider.

Title: Raspberry Pi Synth Board <br />
URL: [URL](https://diyelectromusic.com/2022/09/25/raspberry-pi-v1-model-b-synth-board-part-2/) <br />
Differences: This synthesizer was made using a Raspberry Pi Model B which has builtin I2S interface. Our proton board will need to use PIO to implement the I2S interface. 