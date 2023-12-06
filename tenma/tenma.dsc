
CdImage cd
  cd.addTrackStart(1, "AUDIO")
    cd.addTrackIndex(1)
    cd.addWavAudio("tenma_01.wav")
  cd.addTrackEnd()
  
  cd.addTrackStart(2, "MODE1RAW")
    cd.addModeChange("MODE1")
    cd.addPregapMsf(0, 3, 6)
    cd.addTrackIndex(1)
    cd.addRawData("tenma_02_build.iso")
  cd.addTrackEnd()
  
  cd.addTrackStart(3, "AUDIO")
    cd.addPregapMsf(0, 2, 0)
    cd.addTrackIndex(1)
    cd.addWavAudio("tenma_03.wav")
  cd.addTrackEnd()
  
  cd.addTrackStart(4, "AUDIO")
    cd.addTrackIndex(1)
    cd.addWavAudio("tenma_04.wav")
  cd.addTrackEnd()
  
  cd.addTrackStart(5, "AUDIO")
    cd.addTrackIndex(1)
    cd.addWavAudio("tenma_05.wav")
  cd.addTrackEnd()
  
  cd.addTrackStart(6, "MODE1RAW")
    cd.addModeChange("MODE1")
    cd.addPregapMsf(0, 3, 1)
    cd.addTrackIndex(1)
    cd.addRawData("tenma_06.iso")
  cd.addTrackEnd()
  
cd.exportBinCue("tenma_build")
