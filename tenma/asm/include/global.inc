
;==============================================================================
; HARDWARE
;==============================================================================

.define bytesPerPattern $20

.define patternW 8
.define patternH 8
.define spritePatternW 16
.define spritePatternH 16

.define spriteAttrBaseX 32
.define spriteAttrBaseY 64

.define bytesPerSpritePattern $80
.define bytesPerSpritePatternPlane $20

.define bytesPerPaletteLine 32

.define vce_crLo $400
.define vce_crHi $401
.define vce_ctaLo $402
.define vce_ctaHi $403
.define vce_ctwLo $404
.define vce_ctwHi $405

.define vdp_select $0000
.define vdp_data $0002
  .define vdp_dataLo $0002
  .define vdp_dataHi $0003

.define vdp_regMawr $00
.define vdp_regMarr $01
.define vdp_regVrr  $02
.define vdp_regVwr  $02
.define vdp_regCr   $05
.define vdp_regRcr  $06

.define rcrBaseLine $40

.define builtInMemPage $F8
.define scdBaseMemPage $68

;==============================================================================
; BIOS
;==============================================================================

.define AD_RESET $E030
.define AD_TRANS $E033
.define AD_READ $E036
.define AD_WRITE $E039
.define AD_PLAY $E03C
.define AD_CPLAY $E03F
.define AD_STOP $E042
.define AD_STAT $E045

.define CD_BASE $E006
.define CD_READ $E009
.define CD_PLAY $E012
.define CD_FADE $E02D
.define EX_JOYSNS $E063
.define EX_VSYNC $E07B
.define EX_RCRON $E07E
.define EX_RCROFF $E081
.define EX_IRQON $E084
.define EX_IRQOFF $E087
.define EX_BGON $E08A
.define EX_BGOFF $E08D
.define EX_SPRON $E090
.define EX_SPROFF $E093
.define EX_DSPON $E096
.define EX_DSPOFF $E099
.define EX_SATCLR $E0A2
.define EX_SETWRT $E0AE

.define _A $F8
  .define _AL $F8
  .define _AH $F9
.define _B $FA
  .define _BL $FA
  .define _BH $FB
.define _C $FC
  .define _CL $FC
  .define _CH $FD
.define _D $FE
  .define _DL $FE
  .define _DH $FF

.define JOY $2228
.define JOYTRG $222D

;==============================================================================
; FONT
;==============================================================================

.define bytesPerRawFontChar 10
.define rawFontCharW 8
.define rawFontCharH 10
.define fullFontCharW 16
.define fullFontCharH 16
.define fullFontCharBytesPerLine 2

.define fontNumFillerLines (fullFontCharH-rawFontCharH)
.define fontFillerSize (fontNumFillerLines*fullFontCharBytesPerLine)

/*.define fontBaseOffset $20
.define fontMaxChars $60
.define fontDigitBaseOffset $21
;.define limitedFontMaxChars $5C
;.define digitBaseOffset $51
;.define digitZeroOffset $51
;.define digitSpaceOffset $50
;.define nullCharOffset $9F

.define code_end       $00
.define code_linebreak $01
.define code_space     $20

.define fontEmphToggleOp $7F

.define code_dteBase fontBaseOffset+fontMaxChars*/

;==============================================================================
; SCENE
;==============================================================================

.define advSceneBaseBank $78
.define advSceneBlockStartPtr $BFFC
.define advScenePointerBlockOffset 0
.define advScenePresentMarkerBlockOffset 2
.define advScenePresentMarkerA $53
.define advScenePresentMarkerB $42

;==============================================================================
; SCENE FONT
;==============================================================================

.define linesPerRawSceneFontChar 10
.define bytesPerSceneFontChar 10
.define numSceneFontChars $51

.define sceneFontCharsBase $20

/*; this is where the expanded scene executable is placed on disc
.define newSceneExeSectorNum $2E1A
; minus 2 because the last two sectors are reserved for
; scene-specific subtitles
.define newSceneExeSectorSize $C

.define subtitleDataBaseSectorLo $22
.define subtitleDataBaseSectorMid 0
.define subtitleDataBaseSectorHi 0
.define subtitleDataSectorCount 2
.define subtitleDataLoadAddr $9000*/

.define defaultSubtitleBaseY $B8
.define defaultSubtitleGroupTopToBottomGap $A0

; offset the raw font character by this many lines within the
; 16x16 areas that are actually rendered
;.define rawFontCharYOffset 3

;==============================================================================
; LINE INTERRUPTS
;==============================================================================

.define newRcrPicEndLine $90
.define newRcrStartLine $A8
.define newRcrEndLine newRcrStartLine+32

.define newRcr_state_waitingForOrigEnd 0
.define newRcr_state_waitingForPicEnd 1
.define newRcr_state_waitingForNewStart 2
.define newRcr_state_waitingForNewEnd 3

;==============================================================================
; VARIABLES
;==============================================================================

.define sceneTimer $3192
  .define sceneTimerLo $3192
  .define sceneTimerHi $3193

;==============================================================================
; TEXT
;==============================================================================

; font will be included into free space
;.define fontLoadType_normal 0
; font must be preloaded into a (static, guaranteed paged-in) area
;.define fontLoadType_preload 1
