set -o errexit

make adpcmdmp_pce

# block of mainly sound effects
#./adpcmdmp_pce tenma_02.iso 0x0C00 1024 test_adpcm.wav --rate 16000 --smart

# !?!? adpcm music??? there's a whole track in here!! sounds like something recorded
# off a PC98!?
# this IMMEDIATELY precedes the first valid map on the disc (tsuchiura castle, at 0x1137)
#./adpcmdmp_pce tenma_02.iso 0x10B0 0x87 test_adpcm.wav --rate 16000 --smart

# next is a map block, which seems to have had unused sectors filled with "random"
# data, including tiny fragments of adpcm data.
# more music, voices (some at the wrong sample rate)...
# but only in tiny sub-second bursts
#./adpcmdmp_pce tenma_02.iso 0x1000 0x400 test_adpcm.wav --rate 16000 --smart

# more stuff, a bunch of tiny sound effects?
#./adpcmdmp_pce tenma_02.iso 0x2000 0x400 test_adpcm.wav --rate 16000 --smart

# has a couple fragments near the end
#./adpcmdmp_pce tenma_02.iso 0x2800 0x400 test_adpcm.wav --rate 16000 --smart

# more fragments, one longer clip (that gets cut off), then more fragments.
# tsunade saying "ziria!" and "jipang"?
#./adpcmdmp_pce tenma_02.iso 0x2C00 0x400 test_adpcm.wav --rate 16000 --smart
# fragments
#./adpcmdmp_pce tenma_02.iso 0x3000 0x400 test_adpcm.wav --rate 16000 --smart
# fragments
#./adpcmdmp_pce tenma_02.iso 0x3400 0x400 test_adpcm.wav --rate 16000 --smart
# long clip that gets cut off, more fragments
#./adpcmdmp_pce tenma_02.iso 0x3800 0x400 test_adpcm.wav --rate 16000 --smart
# fragments
#./adpcmdmp_pce tenma_02.iso 0x3C00 0x400 test_adpcm.wav --rate 16000 --smart
# many clips of someone talking (jashinsai?)
# clip of ziria?
# fragments
#./adpcmdmp_pce tenma_02.iso 0x4000 0x400 test_adpcm.wav --rate 16000 --smart
# fragments
#./adpcmdmp_pce tenma_02.iso 0x4400 0x400 test_adpcm.wav --rate 16000 --smart
# fragments
#./adpcmdmp_pce tenma_02.iso 0x4800 0x400 test_adpcm.wav --rate 16000 --smart

# okay, NOW things get interesting.
# some fragments... then a really goddamn long monologue from ba'al daimon!!
# then lots of voice clips that aren't used
# ("the monsters are gone!", etc.)
#./adpcmdmp_pce tenma_02.iso 0x4C00 0x400 test_adpcm.wav --rate 16000 --smart
#./adpcmdmp_pce tenma_02.iso 0x4CFD 0x370 test_adpcm.wav --rate 16000 --smart
# continuation of previous, then other stuff
#./adpcmdmp_pce tenma_02.iso 0x5000 0x400 test_adpcm.wav --rate 16000 --smart

# ziria?: "yabee!" "shishou!"
# and then, FINALLY, some actually used clips (gouyoku trying to buy tsunade).
# i think the rest of this is the adpcm data they actually intended to include
# in the game, and continues on for some time.
#./adpcmdmp_pce tenma_02.iso 0x6400 0x400 test_adpcm.wav --rate 16000 --smart

# after much used dialogue, which appears to be sorted by character (good thing
# nobody in this game ever has a voiced conversation or it'd be really awkward!)
# we get to the narration for the intro.
# it has a bunch of really long blank spaces between lines for some reasons.
# then, interestingly, we have the narrator reading the lines shown for beating
# masakado ("ジライア　ツナデ　オロチ丸は　ついに　大門教を　倒し・・・").
# i guess that was supposed to be one of the ending "slides" at some point.
# then there's the narration for the unused ending "slides".
#./adpcmdmp_pce tenma_02.iso 0x9400 0x400 test_adpcm.wav --rate 16000 --smart
#./adpcmdmp_pce tenma_02.iso 0x97CE 0x400 test_adpcm.wav --rate 16000 --smart

#./adpcmdmp_pce tenma_02.iso 0x9C00 0x400 test_adpcm.wav --rate 16000 --smart

# end of the used voice acting block
#./adpcmdmp_pce tenma_02.iso 0xA800 0x400 test_adpcm.wav --rate 16000 --smart

# clips from previous version of game included for attract mode
# (some are not used, including three long clips of "battle" background noise
# that are obviously meant for the scenes of the burning city)
#./adpcmdmp_pce tenma_02.iso 0xB400 0x400 test_adpcm.wav --rate 16000 --smart
#./adpcmdmp_pce tenma_02.iso 0xB69E 0x400 test_adpcm.wav --rate 16000 --smart
./adpcmdmp_pce tenma_02.iso 0xB724 0x400 test_adpcm.wav --rate 16000 --smart
