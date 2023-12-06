
;==============================================================================
; 
;==============================================================================

;.include "include/global.inc"

;===================================
; misc
;===================================

.define sceneOp_terminator $00
.define sceneOp_waitForFrame $01
.define sceneOp_br $02
.define sceneOp_subsOffNoClear $03
.define sceneOp_subsOnNoClear $04
.define sceneOp_swapAndShowBuf $05
.define sceneOp_subsOff $06
.define sceneOp_finishCurrentLine $07
.define sceneOp_setPalette $08
.define sceneOp_writeMem $09
.define sceneOp_setCropOn $0A
.define sceneOp_startNewString $0B
.define sceneOp_waitForSyncVar $0C
;.define sceneOp_writeVram $0D
.define sceneOp_writePalette $0D
.define sceneOp_jump $0E
.define sceneOp_queueSubsOff $0F
.define sceneOp_jumpIfMaskedEq $10
.define sceneOp_writeMemWord $11
.define sceneOp_setBoxFadeLevel $12
.define sceneOp_setLineFloodMode $13
.define sceneOp_jumpIfLastSyncIdEq $14
.define sceneOp_swapBuf $15

.define numSubtitleCompBufferLines 4
.define spritesPerSubtitleCompLineBuffer 8
.define maxNumSubtitleSprites spritesPerSubtitleCompLineBuffer*numSubtitleCompBufferLines
; allocate an extra sprite per line to allow for the outlining algorithm
; (which uses two extra pixel columns, and possibly needs an extra pattern.
; the extra allocation here is not actually USED, but ensures the generated
; outline gets sent.)
.define bytesPerSubtitleCompLineBuffer bytesPerSpritePatternPlane*(spritesPerSubtitleCompLineBuffer+1)
.define sizeOfAllSubtitleCompBuffers numSubtitleCompBufferLines*bytesPerSubtitleCompLineBuffer

; we use a 24-pixel wide area to compose the character data,
; i.e. 3 bytes per line
.define bytesPerSubtitleCharCompBufferLine 3
.define subtitleCharCompBufferSize linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine

; sprites are 16 pixels tall, but the font is only 10 pixels high,
; so we apply some padding at the top to make things more centered
;.define numSubtitleFontCharTopPaddingLines 3
; ok but see there's this scene in the intro where we hit the sprite limit on
; exactly two lines at the bottom of the subtitles...
; and we have exactly two lines of padding past the normal point that we can
; use to shift them down while moving the sprites up to keep the text in the
; same relative position...
; so let's do that
; (also, problem accidentally solved by dropping the space from "majide",
; but might as well keep the new alignment at this point)
.define numSubtitleFontCharTopPaddingLines 3
.define numSubtitleFontCharBottomPaddingLines spritePatternH-linesPerRawSceneFontChar-numSubtitleFontCharTopPaddingLines

.define satVramAddr $7F00

; G/R/B
.define defaultSubColor ($7<<3)|($7<<6)|($7)
.define defaultSubShadowColor $0000

; FIXME: this generates more than the 32 sprites than can fit in the queue
; if flood mode is on and all 4 possible lines are used
.define lineFloodModeTargetSpriteCount 9

.define sceneOp_writeMem_pointerBase $C000

.define sceneOp_writeVram_pointerBase $A000

;=====
; speed settings.
; if an action is reliably fast enough,
; it's possible to handle more than one per vsync cycle
;=====

; maximum number of script ops to handle per cycle
; (including character literals, which are costly to render,
; hence the limit of 1)
;.define maxScriptActionsPerIteration 2
; max sprite attributes to generate per cycle
;.define maxSpriteAttrTransfersPerIteration 4
; max sprite graphics to transfer to vram per cycle
;.define maxSpriteGrpTransfersPerIteration 2

;=============================
; structs
;=============================

.struct SubtitleCompBufferLineState
  startPtr              dw
  pixelW                db
  numPatterns           db
  ; the fields below are used during queue evaluation
  ; during composition, this is set to the number of
  ; patterns remaining to be converted and sent
  patternTransfersLeft  db
  ; current getpos
  currentPtr            dw
.endst

.struct SpriteAttribute
  y             dw
  x             dw
  pattern       dw
  flags         dw
.endst

.struct VramTransferStatus
  transferOn            db
  currentStatePtr       dw
  vramPutPos            dw
  lineNum               db
.endst

;===================================
; macros for commonly-needed patch operations
;===================================

.macro fixSprOff ARGS addr
  .bank 0 slot 0
  .orga addr
  .section "fixSprOff \@" overwrite
    ; instead of turning sprites off entirely,
    ; just clear non-subtitle ones out
    ; (NOTE: this takes effect one frame later
    ; than shutting them off entirely would,
    ; so use caution)
    jsr clearAndSendSubtitleExclusionOverwrite
  .ends
.endm

.macro fixDspOff ARGS addr
  .bank 0 slot 0
  .orga addr
  .section "fixDspOff \@" overwrite
    ; turn off only the background
    jsr EX_BGOFF
  .ends
.endm

.macro fixDspOffWithSprClrAndSync ARGS addr
  .bank 0 slot 0
  .orga addr
  .section "fixDspOffWithSprClrAndSync \@" overwrite
    ; swap the order of the operations here --
    ; clear non-sub sprites, wait for sync so
    ; that the effect kicks in the frame after that,
    ; then turn BG off.
    ; this ensures that all layers disappear simultaneously
    ; instead of the sprites disappearing one frame after
    ; the background
    jsr clearAndSendSubtitleExclusionOverwrite
    jsr waitForSync
    jsr EX_BGOFF
  .ends
.endm

.macro fixSprOff_doVarSync ARGS addr
  .bank 0 slot 0
  .orga addr
  .section "fixSprOff_doVarSync \@" overwrite
    ; instead of turning sprites off entirely,
    ; just clear non-subtitle ones out
    ; (NOTE: this takes effect one frame later
    ; than shutting them off entirely would,
    ; so use caution)
    jsr incrementSyncVarCounterAndSprClr
  .ends
.endm

.macro fixDspOff_doVarSync ARGS addr
  .bank 0 slot 0
  .orga addr
  .section "fixDspOff \@" overwrite
    ; turn off only the background
    jsr incrementSyncVarCounterAndBgOff
  .ends
.endm

.macro fixDspOffWithSprClrAndSync_doVarSync ARGS addr
  .bank 0 slot 0
  .orga addr
  .section "fixDspOffWithSprClrAndSync_doVarSync \@" overwrite
    ; swap the order of the operations here --
    ; clear non-sub sprites, wait for sync so
    ; that the effect kicks in the frame after that,
    ; then turn BG off.
    ; this ensures that all layers disappear simultaneously
    ; instead of the sprites disappearing one frame after
    ; the background
    jsr clearAndSendSubtitleExclusionOverwrite
    jsr waitForSync
    jsr incrementSyncVarCounterAndBgOff
  .ends
.endm

;===================================
; string insert op
;===================================

.macro SCENE_addString ARGS filename
  .incbin filename
  ; terminator
  .db $07
.endm

;===================================
; scene op macros
;===================================

; FIXME: i'm pretty sure the pce cpu is not clocked
; at exactly 60 frames per second, but is this correct?
;.define framesPerSecond 59.9400599401
;.define framesPerSecond 60
;.define framesPerSecond 60.2585034014
; if you're wondering what the hell this number is:
; i empirically determined while working on the previous game that i had to
; speed up my reference recordings from 41000 hz to 44290 hz to get them to sync
; with the actual game. so this is (41000/44290) * 60.
; somehow, apparently, it works(?)
; too fast?
;.define framesPerSecond 59.7426055543
.define framesPerSecond 59.7426055543+0.08

.macro cut_resetCompBuffers
  .db sceneOp_resetCompBuffers
.endm

.macro cut_setPalette ARGS palIndex
  .db sceneOp_setPalette
    .db palIndex
.endm

.macro cut_waitForFrame ARGS time
  .if (time+SYNC_offset) - SYNC_time < 0
;    .fail
    .db sceneOp_waitForFrame
      .dw 0
  .else
    .db sceneOp_waitForFrame
      .dw ((time+SYNC_offset) - SYNC_time)
  .endif
.endm

; audacity can report times in absolutely goddamn every format there is...
; except 60fps frames
.macro cut_waitForFrameMinSec ARGS min sec
  cut_waitForFrame ((min*60)+sec)*framesPerSecond
.endm

.macro cut_prepAndSendGrp ARGS tileIndex
  .db sceneOp_prepAndSendGrp
    .dw tileIndex
.endm

.macro cut_swapAndShowBuf
  .db sceneOp_swapAndShowBuf
.endm

.macro cut_subsOff
  .db sceneOp_subsOff
.endm

/*.macro cut_writeMem ARGS bank bankOffset byteVal
  .db sceneOp_writeMem
    .db bank
    .dw bankOffset+sceneOp_writeMem_pointerBase
    .db byteVal
.endm

.macro cut_writeMemWord ARGS bank bankOffset wordVal
  .db sceneOp_writeMemWord
    .db bank
    .dw bankOffset+sceneOp_writeMem_pointerBase
    .dw wordVal
.endm*/

.macro cut_writeMem ARGS bank bankOffset byteVal
  .db sceneOp_writeMem
    .db bank
    .dw bankOffset
    .db byteVal
.endm

.macro cut_writeMemWord ARGS bank bankOffset wordVal
  .db sceneOp_writeMemWord
    .db bank
    .dw bankOffset
    .dw wordVal
.endm

.macro cut_setCropOn ARGS val
  .db sceneOp_setCropOn
    .db val
.endm

.macro cut_setLowPrioritySprObjOffset ARGS offset
  .db sceneOp_setLowPrioritySprObjOffset
    .db offset
.endm

.macro cut_resetSyncTimerFromAdpcm
  .db sceneOp_resetSyncTimerFromAdpcm
.endm

.macro cut_waitForAdpcm ARGS counter
  .db sceneOp_waitForAdpcm
    .db counter
;    .dw time
.endm

.macro cut_writePalette ARGS dst sz
  .db sceneOp_writePalette
    .dw sz
    .dw dst
.endm

/*.macro cut_writeVram ARGS srcBank offset dst sz
  .db sceneOp_writeVram
    .db srcBank
    .dw offset
    .dw sz
    .dw dst
.endm*/

.macro cut_queueSubsOff ARGS time
  .if (time+SYNC_offset) - SYNC_time < 0
    .fail
  .endif

  .db sceneOp_queueSubsOff
    .dw ((time+SYNC_offset) - SYNC_time)
.endm

.macro cut_terminator
  .db sceneOp_terminator
.endm

.macro cut_startNewString ARGS dstaddr
  .db sceneOp_startNewString
    .dw dstaddr
.endm

.macro cut_subsOffNoClear
  .db sceneOp_subsOffNoClear
.endm

.macro cut_subsOnNoClear
  .db sceneOp_subsOnNoClear
.endm

.macro cut_jump ARGS dst
  .db sceneOp_jump
    .dw dst
.endm

.macro cut_waitForSyncVar ARGS counter
  .db sceneOp_waitForSyncVar
    .db counter
.endm

.macro cut_jumpIfMaskedEq ARGS addr mask cmpValue dst
  .db sceneOp_jumpIfMaskedEq
    .dw addr
    .db mask
    .db cmpValue
    .dw dst
.endm

.macro cut_setBoxFadeLevel ARGS level
  .db sceneOp_setBoxFadeLevel
    .db level
.endm

.macro cut_setLineFloodMode ARGS mode
  .db sceneOp_setLineFloodMode
    .db mode
.endm

.macro cut_jumpIfLastSyncIdEq ARGS value target
  .db sceneOp_jumpIfLastSyncIdEq
    .db value
    .dw target
.endm

.macro cut_swapBuf
  .db sceneOp_swapBuf
.endm

;===================================
; scene timing hack(?)
;===================================

;.print "here1"
.define SYNC_time 0
.define SYNC_offset 0

.macro SYNC_setTime ARGS newtime
;  .print "here2"
  .redefine SYNC_time newtime
.endm

.macro SYNC_adpcmTime ARGS index newtime
;  .print "here3"
;  .redefine SYNC_time time
;  cut_resetSyncTimer
  
;  cut_resetSyncTimerFromAdpcm (newtime-SYNC_time)
  cut_waitForAdpcm index
;  cut_resetSyncTimerFromAdpcm
  .redefine SYNC_time newtime
.endm

.macro SYNC_varTime ARGS index newtime
;  .print "here4"
  cut_waitForSyncVar index
  .redefine SYNC_time newtime
.endm



.define SCENE_autoPlaceBaseAddr $01C0
.define SCENE_autoPlaceSeparation $18
.define SCENE_autoPlaceParity 0

.define SCENE_currentAutoPlaceAddr SCENE_autoPlaceBaseAddr

.macro SCENE_startNewStringAuto
  cut_startNewString SCENE_currentAutoPlaceAddr
  
  .ifeq SCENE_autoPlaceParity 0
;    cut_prepAndSendGrp SCENE_autoPlaceBaseAddr
    .redefine SCENE_currentAutoPlaceAddr SCENE_currentAutoPlaceAddr+SCENE_autoPlaceSeparation
    .redefine SCENE_autoPlaceParity 1
  .else
;    cut_prepAndSendGrp SCENE_autoPlaceBaseAddr+SCENE_autoPlaceSeparation
    .redefine SCENE_currentAutoPlaceAddr SCENE_currentAutoPlaceAddr-SCENE_autoPlaceSeparation
    .redefine SCENE_autoPlaceParity 0
  .endif
.endm

.macro SCENE_setUpAutoPlace ARGS baseAddr separation
  .redefine SCENE_autoPlaceBaseAddr baseAddr
  .redefine SCENE_autoPlaceSeparation separation
  SCENE_resetAutoPlaceParity
.endm

.macro SCENE_resetAutoPlaceParity
  .redefine SCENE_autoPlaceParity 0
  .redefine SCENE_currentAutoPlaceAddr SCENE_autoPlaceBaseAddr
.endm




; NOTE: maximum delay is 119 frames
.macro SCENE_textBoxOffWithDelay ARGS delay
  cut_setBoxFadeLevel 8+delay
.endm

; NOTE: maximum delay is 120 frames
.macro SCENE_textBoxOnWithDelay ARGS delay
  cut_setBoxFadeLevel (-8)-delay
.endm

.macro SCENE_textBoxOff
  SCENE_textBoxOffWithDelay 0
.endm

.macro SCENE_textBoxOn
  SCENE_textBoxOnWithDelay 0
.endm


