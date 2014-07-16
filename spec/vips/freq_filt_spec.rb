require "spec_helper"

describe VIPS::Image do
  before :all do
    @image = simg('wagon.v')
  end

  it "should perform a FFT on an image", :vips_lib_version => "> 7.23" do
    im = @image.fwfft
    im.abs.scaleps.rotquad.should match_sha1('72ea6aa0808097376a8d58a18d43220a683c6f1f')
  end

  it "should perform an inverse FFT" do
    im = @image.fwfft.invfft
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.abs.scaleps.rotquad.should match_sha1('709ffff085b09824ea9c96ab2a548c4eb5b0f61c')
    end
  end

  it "should perform an inverse FFT to real space" do
    im = @image.fwfft.invfftr
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.abs.scaleps.rotquad.should match_sha1('709ffff085b09824ea9c96ab2a548c4eb5b0f61c')
    end
  end

  it "should perform quad rotation on an image", :vips_lib_version => "> 7.23" do
    im = @image.rotquad
    im.should match_sha1('a2d63a4b32b54f6ab7b44d9f27498797ee7dc0db')
  end

  it "should create an ideal highpass image mask" do
    im = VIPS::Image.fmask_ideal_highpass 32, 32, 0.5
    im.scaleps.should match_sha1('a8e645ce351b88632bd2fe1c19f34d0848178a57')
  end

  it "should create an ideal lowpass image mask" do
    im = VIPS::Image.fmask_ideal_lowpass 32, 32, 0.5
    im.scaleps.should match_sha1('f63d9868f8bc15b14a5ef89ee4e0dd5a6cf4a758')
  end

  it "should create a butterworth highpass image mask" do
    im = VIPS::Image.fmask_butterworth_highpass 32, 32, 42, 0.5, 0.5
    im.scaleps.should match_sha1('97c262105f00ec8020795a2e15aa9a3fcc9a5b8e')
  end

  it "should create a butterworth lowpass image mask" do
    im = VIPS::Image.fmask_butterworth_lowpass 32, 32, 42, 0.5, 0.5
    im.scaleps.should match_sha1('2beeb3cc5619f6dffd95d50ae09801823311b03f')
  end

  it "should create a gauss highpass image mask" do
    im = VIPS::Image.fmask_gauss_highpass 32, 32, 0.5, 0.5
    im.scaleps.should match_sha1('e8ed18921204c1c223df522112b38c5052742ad7')
  end

  it "should create a gauss lowpass image mask" do
    im = VIPS::Image.fmask_gauss_lowpass 32, 32, 0.5, 0.5
    im.scaleps.should match_sha1('32a89277019852d37e7f044c5a88d2f705adc460')
  end

  it "should create an ideal ringpass image mask" do
    im = VIPS::Image.fmask_ideal_ringpass 32, 32, 0.5, 0.3
    im.scaleps.should match_sha1('e88c0ab8709f51e1833fb36af06ed160ecd4e6ac')
  end

  it "should create an ideal ringreject image mask" do
    im = VIPS::Image.fmask_ideal_ringreject 32, 32, 0.5, 0.3
    im.scaleps.should match_sha1('c696b7709380ced45feef1c5a24585b499dbf8d8')
  end

  it "should create a butterworth ringpass image mask" do
    im = VIPS::Image.fmask_butterworth_ringpass 32, 32, 2, 0.5, 0.3, 0.7
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('30838c31af7872b69be6678236b05b2b3611b72b')
    end
  end

  it "should create a butterworth ringreject image mask" do
    im = VIPS::Image.fmask_butterworth_ringreject 32, 32, 2, 0.5, 0.3, 0.7
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('708eaadfed1eede305f5386f503371d4a44f359c')
    end
  end

  it "should create a gauss ringpass image mask" do
    im = VIPS::Image.fmask_gauss_ringpass 32, 32, 0.5, 0.2, 0.5
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('c4a2eb833eaab64f00b8bbbe0ac164308aebcec2')
    end
  end

  it "should create a gauss ringreject image mask" do
    im = VIPS::Image.fmask_gauss_ringreject 32, 32, 0.5, 0.2, 0.5
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('7a8ca3995a39c9d1041db34174ae7576fcead59d')
    end
  end

  it "should create an ideal bandpass image mask" do
    im = VIPS::Image.fmask_ideal_bandpass 32, 32, 0.5, 0.4, 20
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('60cacbf3d72e1e7834203da608037b1bf83b40e8')
    end
  end

  it "should create an ideal bandreject image mask" do
    im = VIPS::Image.fmask_ideal_bandreject 32, 32, 0.5, 0.4, 20
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('059a104f98f5658171c48a4d6b0d39036f953264')
    end
  end

  it "should create a butterworth bandpass image mask" do
    im = VIPS::Image.fmask_butterworth_bandpass 32, 32, 2, 0.5, 0.4, 20, 0.7
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('5334a257ccd51517af9991c53d312490b9381240')
    end
  end

  it "should create a butterworth bandreject image mask" do
    im = VIPS::Image.fmask_butterworth_bandreject 32, 32, 2, 0.5, 0.4, 20, 0.7
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('6e34ef8d76138f7df3882d796d5d343758cb80c4')
    end
  end

  it "should create a gauss bandpass image mask" do
    im = VIPS::Image.fmask_gauss_bandpass 32, 32, 0.5, 0.4, 20, 0.5
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('f8be20f1071abeefcbe7090d2cdfcf5e593e1787')
    end
  end

  it "should create a gauss bandreject image mask" do
    im = VIPS::Image.fmask_gauss_bandreject 32, 32, 0.5, 0.4, 20, 0.5
    # 7.38 has revised fft stuff
    if Spec::Helpers.match_vips_version("> 7.38")
      im.scaleps.should match_sha1('ab86d1f2bffc8acc8289bb42ab87d32e96890ebd')
    end
  end

  it "should create a fractal filter image mask" do
    im = VIPS::Image.fmask_fractal_flt 32, 32, 2.5
    im.scaleps.rotquad.should match_sha1('ee2e68593a0407c938b139634e4208eea7c38140')
  end

  it "should apply frequency filter image masks" do
    im = @image.extract_area(30, 10, 128, 128)

    mask_lo = VIPS::Image.fmask_ideal_highpass 128, 128, 0.1
    image_lo = im.freqflt(mask_lo)

    mask_hi = VIPS::Image.fmask_ideal_lowpass 128, 128, 0.1
    image_hi = im.freqflt(mask_hi)

    combined = image_lo.add(image_hi).lin(0.5, 0)
    combined.should match_sha1('8800da7d7ca039c6b7121858741a535e90ceacfb')
  end

  it "should display a power spectrum of an image", :vips_lib_version => "> 7.23" do
    im = @image.disp_ps
    im.should match_sha1('72ea6aa0808097376a8d58a18d43220a683c6f1f')
  end

  it "should calculate the phase correlation between two images" do
    im1 = @image.extract_area(30, 10, 128, 128)
    im2 = @image.extract_area(40, 25, 128, 128)

    phasecor = im1.phasecor_fft(im2)
    maxpos = phasecor.maxpos
    maxpos[0].should == 10
    maxpos[1].should == 15
  end

  it "should create a random fractal image" do
    im = VIPS::Image.fractsurf 512, 2.5
  end
end
