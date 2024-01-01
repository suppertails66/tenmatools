
.include "include/global.inc"

; enable for walk through walls and no encounters when running,
; and 65535 EXP given after battle if button 2 held
.define TRANSLATION_DEBUG 0
; enable to boot to credits
.define CREDITS_DEBUG 0
; enable to boot to "the end" screen
; must also activate CREDITS_DEBUG for this to work
.define THEEND_DEBUG 0
; enable to boot to "folk tale classics"
.define THEATER_DEBUG 0

; could someone please rewrite wla-dx to allow dynamic bank sizes?
; thanks
.memorymap
   defaultslot     0
   
   slotsize        $10000
   slot            0       $0000
.endme

.define sizeOfMetabank $10000
.define std_slot 0
.define numMetabanks 22

.define metabank_empty 0
.define metabank_kernel 1
.define metabank_overw 2
.define metabank_shop 3
.define metabank_battle 4
.define metabank_unknown1 5
.define metabank_battle_extra 6
.define metabank_battle_text 7
.define metabank_battle_text2 8
.define metabank_unknown2 9
.define metabank_intro 10
.define metabank_saveload 11
.define metabank_generic_text 12
.define metabank_credits 13
.define metabank_special 14
.define metabank_unknown3 15
.define metabank_info 16
.define metabank_masakado 17
; new for translation
.define metabank_new1 18 ; free area 1
.define metabank_new2 19 ; free area 2
.define metabank_new3 20 ; subtitles
.define metabank_new4 21 ; non-super cd error message

.rombankmap
  bankstotal numMetabanks
  
  banksize $10000
  banks numMetabanks
.endro

.emptyfill $FF

.background "main_asm.bin"

;===================================
; unbackgrounds
;===================================



;===================================
; new stuff
;===================================

; 0x1142 = unused space in map 0x1137
;.define newArea1SrcSectorH $00
;.define newArea1SrcSectorM $11
;.define newArea1SrcSectorL $42
.define newArea1SrcSectorH $00
.define newArea1SrcSectorM $00
.define newArea1SrcSectorL $02
.define newArea1SectorCount 7
.define newArea1MemPage $7E
.define newArea1LoadAddr $C000
.define newArea2MemPage $7F
.define newArea2LoadAddr $C000

; number of bytes per map on disc
.define mapDataSize $D000

; banks $68-$75 are used for storing cached map data
; (7 banks per map)
.define mapCacheAMemPage $68
.define mapCacheBMemPage $6F

; banks $76-7B are used to store a copy of the battle module,
; speeding up the start of encounters
.define battleCacheSrcSectorH $00
.define battleCacheSrcSectorM $00
.define battleCacheSrcSectorL $1D
.define battleCacheSectorCount 24
.define battleCacheMemPage $76

; bank $7C stores the last 0x2000 bytes of the overw module.
; the first 0x1000 are stored directly in new1/new2.
.define overwCacheSrcSectorH $00
.define overwCacheSrcSectorM $00
.define overwCacheSrcSectorL $15
.define overwCacheSectorCount 4
.define overwCacheMemPage $7C

; new area 3 overwrites the unused map at 0x2B6B
;.define newArea3SrcSectorH $00
;.define newArea3SrcSectorM $2B
;.define newArea3SrcSectorL $6B
; unused adpcm at 0x10B0
.define newArea3SrcSectorH $00
.define newArea3SrcSectorM $10
.define newArea3SrcSectorL $B0
.define newArea3SectorCount 4
.define newArea3MemPage $7D
.define newArea3LoadAddr $C000

; new area 4 overwrites the unused map at 0x2B6B
;.define newArea4SrcSectorH $00
;.define newArea4SrcSectorM $4C
;.define newArea4SrcSectorL $55
; unused adpcm
.define newArea4SrcSectorH $00
.define newArea4SrcSectorM $10
.define newArea4SrcSectorL $B9
.define newArea4SectorCount 2
.define newArea4MemPage $87
.define newArea4LoadAddr $C000

;.define scdErrorVramSrcSectorH $00
;.define scdErrorVramSrcSectorM $4C
;.define scdErrorVramSrcSectorL $57
.define scdErrorVramSrcSectorH $00
.define scdErrorVramSrcSectorM $10
.define scdErrorVramSrcSectorL $BB
.define scdErrorVramSectorCount 10
.define scdErrorVramLoadAddr $0000

; new ops
.define opcode_globalRedirect $EA
.define opcode_br4 $EB
.define opcode_forceBoxEnd $EC
.define opcode_forceBoxPause $ED
.define opcode_localRedirect $EE
; old ops
.define opcode_printNum $FB
.define opcode_printString $FC
.define opcode_linebreak $FE
.define opcode_terminator $FF

; new menu ops
.define menuOpcode_narrowItem $61
.define menuOpcode_narrowMapName $62
.define menuOpcode_repeat $EB
.define menuOpcode_embeddedString $EC

.define code_space $08
.define code_exclamationMark $49
.define code_questionMark $4A
.define code_rquote $51
.define code_hyphen $52
.define code_ellipsis $53
.define code_dash $55
.define code_rquoteSingle $5E

.define oldOpLowerLimit $EF
.define newOpLowerLimit $E8

.define fontBaseOffset $08
.define fontDigitBase $09

.define code_T_preconverted $26-fontBaseOffset

.define dteEncodingLowerLimit $68
.define dteDictSize $80
.define dteEncodingUpperLimit dteEncodingLowerLimit+dteDictSize

.define maxTextX 216

.define globalRedirectBufferSize 32

.define maxFontId $06
.define fontId_std $00
.define fontId_narrow $01
.define fontId_narrow_smallspace $03
.define fontId_narrower $02
.define fontId_narrower_smallspace $04
; same as fontId_narrow, but with all spacing reduced by 1
.define fontId_narrow_crammed $05
; same as fontId_narrower, but with all spacing reduced by 1.
; the result would probably have been quite unintelligible on contemporary displays...
.define fontId_narrower_crammed $06

.define printToFit_defaultMaxW 64
.define maxNarrowItemNameW 64
; the original game has a 72-pixel area that goes right to
; the right edge of the box.
; we add 16 pixels, but going all the way to the far end with no margin
; looks kind of bad, so we squish the text if it would get too close.
;.define maxNarrowMapNameW 72
.define maxNarrowMapNameW 72+12

;===================================
; old routines
;===================================

.define doStdFrameEnd $40E7
.define showSprite $4469
.define doShortFrameIdle $5C4A
.define incTextPtr $626F
.define printToBox $635E
.define advancePrintPos $64BB
.define doLinebreak $64E7
.define printStringRaw $651B
.define printStringRawAltEntry $651E
.define switchMpr6To85Bank $65E2
.define checkIfBoxEndAllowedYet $84F5
.define stopAdpcmIfNeeded $84DE
.define drawAndRunMenu $8571
.define fetchFromRefedSrc $8613
.define printFetchedCharOrSubContent $8623
.define drawWindowRight $8C47

;===================================
; old memory locations
;===================================

.define gameModeB $00

.define textPtrB $7B
  .define textPtrLoB textPtrB+0
  .define textPtrHiB textPtrB+1

.define freeBankMpr6B $85

.define menuCurrentStringStartLoB $2B
.define menuCurrentStringStartHiB $2C
.define menuBaseXB $CB
.define menuBaseX $20CB
.define menuBaseYB $CC
.define menuBaseY $20CC
.define menuCurrentTargetXB $D1
.define menuCurrentTargetYB $D2
.define menuCurrentStringIndexB $D3

.define forceNoTextDelayOn $2731

.define allowedSkipButtons $3C4C

.define lastReadOpOrPrintSubstate $33C5

.define nextPrintCodepoint $33C7
  .define nextPrintCodepointLo nextPrintCodepoint+0
  .define nextPrintCodepointHi nextPrintCodepoint+1

.define fontCharBufferStart $33C9
; one extra padding row for new font
;.define fontCharBufferNonPaddingStart fontCharBufferStart+4
.define fontCharBufferNonPaddingStart fontCharBufferStart+6
.define fontCharBufferSize $20

.define textCoarseX $33ED
.define textCoarseY $33EE
.define textFineX $33F0
.define currentBoxLineCount $33F4

.define currentPrintMode $33F5

.define stringScriptQueue $33FB
  .define stringScriptQueueLo stringScriptQueue+0
  .define stringScriptQueueHi stringScriptQueue+1
.define stringScriptQueuePos $33FF
.define substringPrintActive $3400
.define stringSubstringMemBlockStart stringScriptQueue
.define stringSubstringMemBlockSize (substringPrintActive-stringScriptQueue)+1

.define textSpeedSetting $3404

.define currentWindowW $3C0A
.define currentWindowH $3C0B
.define lastWindowByte $3C0C

; for menu substrings
.define strByteSize $3C10
.define strTileCount $3C11

.define autoBattleOn $4149

;===================================
; macros
;===================================

.macro doTrampolineCall ARGS bank dst
  jsr trampolineCall
    .dw dst
    .db bank
.endm

.macro doTrampolineCallNew1 ARGS dst
  jsr trampolineCall
    .dw dst
    .db newArea1MemPage
.endm

.macro doTrampolineCallNew2 ARGS dst
  jsr trampolineCall
    .dw dst
    .db newArea2MemPage
.endm

.macro doTrampolineCallNew3 ARGS dst
  jsr trampolineCall
    .dw dst
    .db newArea3MemPage
.endm

;==============================================================================
; test
;==============================================================================

/*.bank metabank_empty slot 0
.orga $0000
.section "test" overwrite
  .db $00,$01,$02,$03
.ends*/

;==============================================================================
; DEBUG
;==============================================================================

.if TRANSLATION_DEBUG != 0

  ;===================================
  ; DEBUG: disable random encounters
  ; if button 2 held
  ;===================================

  .bank metabank_overw slot std_slot
  .orga $ABCA
  .section "overw: debug noenc 1" overwrite
    jmp doDebugNoencCheck
  .ends

  .bank metabank_overw slot std_slot
  .section "overw: debug noenc 3" free
    doDebugNoencCheck:
      lda JOY.w
      and #$02
      beq +
        ; no encounter
        jmp $ABC5
      +:
      
      ; make up work
      lda $309C.w
      jmp $ABCD
  .ends

  ;===================================
  ; DEBUG: give max exp (65535) after
  ; winning battle if button 2 held
  ;===================================

  .bank metabank_battle slot std_slot
  .orga $B5FA
  .section "battle: debug exp 1" overwrite
    doTrampolineCallNew1 doBattleDebugExpCheck
    rts
  .ends

  .bank metabank_new1 slot std_slot
  .section "battle: debug exp 2" free
    doBattleDebugExpCheck:
      ; make up work
      lda #$C9
      sta $2B.b
      lda #$34
      sta $2C.b
      jsr $B605
      
      ; if button 2 held, set exp to 0xFFFF
      lda JOY
      and #$02
      beq +
        lda #$FF
        sta $81.b
        sta $82.b
        sta $83.b
      +:
      
      rts
  .ends

  ;===================================
  ; DEBUG: walk through walls while
  ; button 2 held
  ;===================================

  .bank metabank_overw slot std_slot
  .orga $95B7
  .section "overw: debug walk through walls 1" overwrite
    jmp doOverwDebugWtwCheck
  .ends

  .bank metabank_overw slot std_slot
  .section "overw: debug walk through walls 2" free
    doOverwDebugWtwCheck:
      ; make up work (set player direction?)
      stx $5B.b
      
      lda JOY.w
      and #$02
      beq +
        jmp $95CB
      +:
      jmp $95C9
  .ends

.endif

.if CREDITS_DEBUG != 0

  ;===================================
  ; DEBUG: boot to credits
  ;===================================

  .bank metabank_kernel slot std_slot
  .orga $47E1
  .section "kernel: debug credits 1" overwrite
    jmp $4849
  .ends

.endif

.if THEEND_DEBUG != 0

  ;===================================
  ; DEBUG: skip credits and go
  ; directly to "the end" screen
  ;===================================

  .bank metabank_intro slot std_slot
  .orga $A19B
  .section "intro: debug the end 1" overwrite
    nop
    nop
    nop
  .ends

.endif

.if THEATER_DEBUG != 0

  ;===================================
  ; DEBUG: go directly to theater
  ; on boot
  ;===================================

  .bank metabank_saveload slot std_slot
  .orga $9012
  .section "saveload: debug theater 1" overwrite
    jsr $8528
  .ends
  
.endif

;==============================================================================
; kernel
;==============================================================================

;===================================
; unbackgrounds
;===================================

; item list
; TODO: properly replace item list by moving to new memory...
.unbackground metabank_kernel*sizeOfMetabank+$66D9 metabank_kernel*sizeOfMetabank+$6C5C
; old computeSjisStringSizeInfo, minus new jump at start
.unbackground metabank_kernel*sizeOfMetabank+$8838 metabank_kernel*sizeOfMetabank+$886E
; now-extraneous workaround for v1.0 card bug 

; disused section of battle load routine
; TODO: i think this could be extended up to around $7E56 without issue if needed
.unbackground metabank_kernel*sizeOfMetabank+$7E17+$10 metabank_kernel*sizeOfMetabank+$7E3D

; bios reimplementations
.unbackground metabank_kernel*sizeOfMetabank+$8E88 metabank_kernel*sizeOfMetabank+$8F76
.unbackground metabank_kernel*sizeOfMetabank+$8F84 metabank_kernel*sizeOfMetabank+$8FE8
.unbackground metabank_kernel*sizeOfMetabank+$82D8 metabank_kernel*sizeOfMetabank+$82FB
.unbackground metabank_kernel*sizeOfMetabank+$8F7A metabank_kernel*sizeOfMetabank+$8F7E

;===================================
; remove bios reimplementations
;===================================

; the original v1.0 system card bios had bugs related to ADPCM playback
; that were fixed in later revisions (apparently, AD_CPLAY would intefere
; with other functions?). to work around this, the game has special checks
; for the BIOS version, and if it's 1.0, the game calls its own fixed
; routines instead.
; as we're changing to SCD, this is no longer needed, so we can reuse the space.

.bank metabank_kernel slot std_slot
.orga $8E85
.section "kernel: no bios reimplementation 1" overwrite
  jmp AD_READ
.ends

.bank metabank_kernel slot std_slot
.orga $8F81
.section "kernel: no bios reimplementation 2" overwrite
  jmp AD_PLAY
.ends

.bank metabank_kernel slot std_slot
.orga $82C6
.section "kernel: no bios reimplementation 3" SIZE 18 overwrite
  ; this routine returns carry clear if the adpcm playback is "not busy".
  ; the original implementation has a cumbersome workaround for an apparent
  ; bug on the v1.0 system card, but we no longer need to worry about that.
  pha 
  phx 
  phy 
    jsr AD_STAT
    cmp #$00
    bne +
    ; adpcm not busy
      clc
      bra @done
    +:
    ; adpcm busy
    sec
    @done:
  ply 
  plx 
  pla 
  rts
.ends

.bank metabank_kernel slot std_slot
.orga $8F77
.section "kernel: no bios reimplementation 4" overwrite
  lda #$03
  rts
.ends

;===================================
; trampoline
;===================================

.bank metabank_kernel slot std_slot
.section "kernel: trampoline 1" free
  ; WARNING: not interrupt safe!
  ;          code that runs from interrupts should handle this separately
  trampolineCall:
    ; save A
    sta @restoreIncomingInstr+1.w
  
    ; fetch call addr from stack
    ; lo
    pla
    ; add 3 to get correct offset
;    clc
;    adc #3
    sta @loadInstruction+1.w
    sta @loadInstruction2+1.w
    ; hi
    pla
;    adc #0
    sta @loadInstruction+2.w
    sta @loadInstruction2+2.w
    
    phx
      ldx #1
      -:
        @loadInstruction:
        lda $0000.w,X
        sta @callInstruction.w,X
        
        inx
        cpx #3
        bne -
      
      @loadInstruction2:
      lda $0000.w,X
      sta @bankInstruction+1.w
    plx
    
    ; load in target bank
    tma #$40
    pha
      @bankInstruction:
      lda #$00
      tam #$40
        
        ; save return address in case the routine we call also
        ; needs to do a trampoline and overwrites it
        lda @loadInstruction+1.w
        pha
        lda @loadInstruction+2.w
        pha
          ; restore A to initial value
          @restoreIncomingInstr:
          lda #$00
          ; call target routine
          @callInstruction:
          jsr $0000
          ; save return value
          sta @restoreRetInstr+1.w
        pla
        sta @loadInstruction+2.w
        pla
        sta @loadInstruction+1.w
    pla
    tam #$40
    
    ; advance return address past pointer
    lda #3
    clc
    adc @loadInstruction+1.w
    sta @loadInstruction+1.w
    bcc +
      inc @loadInstruction+2.w
    +:
    
    ; push return address to stack
    lda @loadInstruction+2.w
    pha
    lda @loadInstruction+1.w
    pha
    
    ; restore return value
    @restoreRetInstr:
    lda #$00
    rts
    
    @temp:
      .db $00
.ends

;===================================
; extra init
;===================================

.bank metabank_kernel slot std_slot
.orga $4050
.section "kernel: extra init 1" overwrite
  jmp doExtraInit
.ends

.bank metabank_kernel slot std_slot
.section "kernel: extra init 2" free
  doExtraInit:
    ; this is an scd-only translation, so verify that bios version is
    ; 3 or greater
    ; EX_GETVER
    ; returns X = major version num, Y = minor
    jsr $E05A
    cpx #3
    bcs @isScd
    @doScdErrorMsg:
    -:
      ; version num < 3: this system can't run the translation.
      ; use CD_EXEC to run the error program.
      ; TODO: make the error program
      ; REC H
      lda #newArea4SrcSectorH
      sta _CL.b
      ; REC M
      lda #newArea4SrcSectorM
      sta _CH.b
      ; REC L
      lda #newArea4SrcSectorL
      sta _DL.b
      ; type = read through MPR6
      lda #6
      sta _DH.b
      ; bank num
      lda #newArea4MemPage
      sta _BL.b
      ; length
      lda #newArea4SectorCount
      sta _AL.b
      ; call CD_EXEC
;      jmp $E00F
      jsr CD_READ
      and #$FF
      bne -
      
      lda #newArea4MemPage
      tam #$40
      jmp scdErrorMsgStart
    @isScd:
    
    -:
      ; read new permanent resources from cd to scd memory
      ; type = mpr6 read
      lda #$06
      sta _DH.b
      ; bank num
      lda #newArea1MemPage
      sta _BL.b
      ; length
      lda #newArea1SectorCount
      sta _AL.b
      ; src sector
      lda #newArea1SrcSectorH
      sta _CL
      lda #newArea1SrcSectorM
      sta _CH
      lda #newArea1SrcSectorL
      sta _DL
      ; load
      jsr CD_READ
      ; loop until read successful
      and #$FF
      bne -
    
    ; run additional init routines from loaded content
    doTrampolineCallNew1 doExtraInit_afterLoad
    
    ; make up work
    jmp $47C3
.ends

.bank metabank_new1 slot std_slot
.section "new1: extra init 3" free
  doExtraInit_afterLoad:
    ; in case anyone would like to play the game in japanese but doesn't
    ; have a non-super system card, here is a code to force the scd error
    ; screen to show up and give the option to play the untranslated game:
    ; hold up+select+button 2 as the game boots.
    lda JOY.w
    and #$16
    cmp #$16
    bne +
      jmp doExtraInit@doScdErrorMsg
    +:
    
    ; ensure the backup track is not used in case of read errors
    ; mode = by track
    lda #$80
    sta _BH.b
    ; set = both base and backup
    lda #$00
    sta _CL.b
    ; track = 2 (bcd)
    lda #$02
    sta _AL.b
    jsr CD_BASE
    
    ; read overw module cache
    -:
      ; type = mpr6 read
      lda #$06
      sta _DH.b
      ; bank num
      lda #overwCacheMemPage
      sta _BL.b
      ; length
      lda #overwCacheSectorCount
      sta _AL.b
      ; src sector
      lda #overwCacheSrcSectorH
      sta _CL
      lda #overwCacheSrcSectorM
      sta _CH
      lda #overwCacheSrcSectorL
      sta _DL
      ; load
      jsr CD_READ
      ; loop until read successful
      and #$FF
      bne -
    
    ; read battle module cache
    -:
      ; type = mpr6 read
      lda #$06
      sta _DH.b
      ; bank num
      lda #battleCacheMemPage
      sta _BL.b
      ; length
      lda #battleCacheSectorCount
      sta _AL.b
      ; src sector
      lda #battleCacheSrcSectorH
      sta _CL
      lda #battleCacheSrcSectorM
      sta _CH
      lda #battleCacheSrcSectorL
      sta _DL
      ; load
      jsr CD_READ
      ; loop until read successful
      and #$FF
      bne -
    
    ; read new area 3
    -:
      ; type = mpr6 read
      lda #$06
      sta _DH.b
      ; bank num
      lda #newArea3MemPage
      sta _BL.b
      ; length
      lda #newArea3SectorCount
      sta _AL.b
      ; src sector
      lda #newArea3SrcSectorH
      sta _CL
      lda #newArea3SrcSectorM
      sta _CH
      lda #newArea3SrcSectorL
      sta _DL
      ; load
      jsr CD_READ
      ; loop until read successful
      and #$FF
      bne -
    rts
.ends

;===================================
; handle new redirect ops:
; standard printing
;===================================

.bank metabank_kernel slot std_slot
.orga $6250
.section "kernel: new redirect ops std read 1" overwrite
  jmp doTextRedirectCheck1
.ends

.bank metabank_kernel slot std_slot
.section "kernel: new redirect ops std read 2" free
  doTextRedirectCheck1:
    cmp #opcode_localRedirect
    beq @doLocalRedirect
    cmp #opcode_globalRedirect
    beq @doGlobalRedirect
      cmp #opcode_br4
      bne @notBr4
        ; any br4 box must be preceded by a dummy br4 command
        ; to activate the mode
        lda fourLineBoxOn.w
        bne +
          inc fourLineBoxOn.w
          bra @br4UpdateDone
        +:
        
        stz textFineX.w
        stz textCoarseX.w
        ; textCoarseY sequence: 0 -> 1(.5) -> 3 -> 4(.5)
        inc textCoarseY.w
        ; note that currentBoxLineCount is not touched,
        ; since it would otherwise automatically generate a box pause
        ; once the third line is broken
        
        lda br4Parity.w
        eor #$80
        sta br4Parity.w
        bne @br4UpdateDone
;        @br4MovedToAligned:
;          bra @br4UpdateDone
        @br4MovedToUnaligned:
          inc textCoarseY.w
        @br4UpdateDone:
        jsr incTextPtr
        jmp $624E
      @notBr4:
      cmp #opcode_forceBoxEnd
      bne @notBoxEnd
        lda fourLineBoxOn.w
        beq +
          jsr prepForBr4End
        +:
        
        ; unlike for box pause, we may not need a linebreak at all.
        ; do nothing once box line >= 2.
        lda currentBoxLineCount.w
        cmp #$02
        bcc @finishForceBr
        jsr incTextPtr
        jmp $624E
      @notBoxEnd:
      cmp #opcode_forceBoxPause
      bne @normal
        lda fourLineBoxOn.w
        beq +
          jsr prepForBr4End
        +:
        
        ; repeat until last line reached,
        ; then do one more linebreak
        lda currentBoxLineCount.w
        cmp #$02
        bcs +
        @finishForceBr:
          jsr decTextPtr
        +:
        ; pretend a linebreak occurred instead of this op
        lda #opcode_linebreak
        ; !! drop through !!
      @normal:
      ; make up work
      sta nextPrintCodepointHi.w
      jmp $6253
    @doLocalRedirect:
    ; loop back to read logic, so it's as though the redirect never even occurred
    bsr doTextRedirect
    jmp $624E
    @doGlobalRedirect:
    jsr doGlobalTextRedirect
    jmp $624E
  
  doTextRedirectCheck2:
    cmp #opcode_localRedirect
    beq @doLocalRedirect
    cmp #opcode_globalRedirect
    beq @doGlobalRedirect
      ; make up work
      sta nextPrintCodepointHi.w
      jmp $6538
    @doLocalRedirect:
    ; loop back to read logic, so it's as though the redirect never even occurred
    bsr doTextRedirect
    jmp $652F
    @doGlobalRedirect:
    jsr doGlobalTextRedirect
    jmp $652F
    
  doTextRedirect:
    phy
      ; get low bye
      ldy #1
      lda (textPtrB.b),Y
      pha
        ; get high byte
        iny
        lda (textPtrB.b),Y
        sta textPtrHiB.b
      pla
      sta textPtrLoB.b
    ply
    rts
    
  decTextPtr:
    lda textPtrLoB.b
    bne +
      dec textPtrHiB.b
    +:
    dec textPtrLoB.b
    rts
  
  prepForBr4End:
    stz textCoarseY.w
    stz fourLineBoxOn.w
    rts
.ends

;===================================
; global redirect buffer
;===================================

.bank metabank_kernel slot std_slot
.section "kernel: global redirect 1" free
  ; will most likely only be used for item names,
  ; so no need to use much space for this
  globalRedirectBuffer:
    .ds globalRedirectBufferSize,$00
  
  ; textptr = ptr to start of redirect seq
  doGlobalTextRedirect:
    ; set textptr to src
    jsr doTextRedirect
    
    @altEntry:
    tma #$40
    pha
      ; page in global content bank
      lda #newArea2MemPage
      tam #$40
      
      ; copy to buffer
      phy
        cly
        -:
          lda (textPtrB.b),Y
          sta globalRedirectBuffer.w,Y
          iny
          cmp #opcode_terminator
          bne -
      ply
    pla
    tam #$40
    
    ; set textptr to buffer
    lda #<globalRedirectBuffer
    sta textPtrLoB.b
    lda #>globalRedirectBuffer
    sta textPtrHiB.b
    
    rts
    
.ends

;===================================
; handle new redirect ops:
; printStringRaw
;===================================

.bank metabank_kernel slot std_slot
.orga $6535
.section "kernel: new redirect ops printStringRaw 1" overwrite
  jmp doTextRedirectCheck2
.ends

; TODO: any changes needed to doOverwScript?

;===================================
; end global redirect:
; printStringRaw
;===================================

/*.bank metabank_kernel slot std_slot
.orga $656B
.section "kernel: end global redirect printStringRaw 1" overwrite
  jmp doEndGlobalRedirectCheck_printStringRaw
.ends

.bank metabank_kernel slot std_slot
.section "kernel: end global redirect printStringRaw 2" free
  doEndGlobalRedirectCheck_printStringRaw:
    ; make up work
    tam #$40
    ; ensure disable bank override disabled
    stz freeBankOverride.w
    rts
.ends*/

;===================================
; 8-bit text read: std
;===================================

/*.bank metabank_kernel slot std_slot
.orga $6264
.section "kernel: 8-bit text std read 1" overwrite
  jsr handle8bitTextLiteral
  jmp $626B
.ends

.bank metabank_kernel slot std_slot
.section "kernel: 8-bit text std read 2" free
  ; at this point, textPtr has already been incremented past
  ; the read character, which is in A and has also been written to
  ; nextPrintCodepointHi
  handle8bitTextLiteral:
    ; check if part of a DTE sequence
    cmp #dteEncodingLowerLimit
    bcc @done
    @isDte:
      
    @done:
    rts
    
  decTextPtr:
    lda textPtrLoB.b
    bne +
      dec textPtrHiB.b
    +:
    dec textPtrLoB.b
    rts
  
  dteFlag:
    .db $00
.ends*/

.bank metabank_kernel slot std_slot
.orga $6264
.section "kernel: 8-bit text std read 1" overwrite
;  doTrampolineCallNew1 handle8bitTextLiteral
;  nop
  jsr handle8bitTextLiteral_stdPrint
  jmp $626B
.ends

.bank metabank_kernel slot std_slot
.section "kernel: 8-bit text std read 2" free
  dteSeqActive:
    .db $00
  selectedFontId:
    .db $00
  currentCharWidth:
    .db $00
  ; TODO: never used? remove?
;  currentCharRawGlyphWidth:
;    .db $00
  
  handle8bitTextLiteral:
    cmp #dteEncodingLowerLimit
    bcc @done
      doTrampolineCallNew1 handle8bitTextLiteral_sub
    @done:
    rts
  
  handle8bitTextLiteral_stdPrint:
    @loop:
      pha
        ; peek at next char, fetching from DTE seq if needed
        cmp #dteEncodingLowerLimit
        bcc +
          doTrampolineCallNew1 doDteLookup
        +:
        
        ; if not a space, handle as normal
        cmp #code_space
        beq +
        @stdExit:
          pla
          bra handle8bitTextLiteral
        +:
        
        lda freeBankMpr6B.b
        pha
          tma #$40
          sta freeBankMpr6B.b
          ; check for line overflow, insert linebreak if needed,
          ; and skip past any pending spaces
          doTrampolineCallNew1 checkAndHandleAutoWordWrap
          tax
        pla
        sta freeBankMpr6B.b
        txa
        
        ; if result was zero, no break occurred, and we should print as normal
        and #$FF
        bne +
          jsr incTextPtr
          bra @stdExit
        +:
      pla
      
      ; loop back to base handler start (allowing ops to be handled
      ; if encountered)
      ; ** DESTROY CALL **
;      pla
;      pla
;      jmp $624E
      
      ; pretend a linebreak occurred instead of this
      lda #opcode_linebreak
      sta nextPrintCodepointHi.w
      ; ** DESTROY CALL **
      pla
      pla
      jmp $6259
.ends

.bank metabank_new1 slot std_slot
.section "kernel: 8-bit text std read 3" free
  ; TODO: having to do a full trampoline call for dte lookup is slow.
  ; if possible, put dedicated code in main kernel area to load in this bank
  ; and do the lookup.
  
  ; at this point, textPtr has already been incremented past
  ; the first read character, which is in A and has also been written to
  ; nextPrintCodepointHi.
  ; our goal is to not increment textPtr (easy enough)
  ; and also to check for DTE sequences, replacing the character in
  ; nextPrintCodepointHi if needed, and rewind textptr to the
  ; previous character if a new sequence was started.
  handle8bitTextLiteral_sub:
    ; check if part of a DTE sequence
;    cmp #dteEncodingLowerLimit
;    bcc @done
;    @isDte:
      bsr doDteLookup
      sta nextPrintCodepointHi.w
      
      @altEntry:
      ; flip dte flag
      lda dteSeqActive.w
      eor #$FF
      sta dteSeqActive.w
      
      ; if new sequence started, rewind stream so encoding byte is seen again
      ; next time
      beq +
        jmp decTextPtr
      +:
    @done:
    rts
  
  doDteLookup:
    sec
    sbc #dteEncodingLowerLimit
    asl
    phx
      tax
      lda dteSeqActive.w
      beq +
        inx
      +:
      lda dteTable.w,X
    plx
    rts
.ends

;===================================
; 8-bit text read: printStringRaw
;===================================

.bank metabank_kernel slot std_slot
.orga $653B
.section "kernel: 8-bit text printStringRaw 1" overwrite
;  doTrampolineCallNew1 handle8bitTextLiteral
;  nop
;  nop
  jsr handle8bitTextLiteral
  jmp $6543
.ends

;===================================
; use new font
;===================================

.bank metabank_kernel slot std_slot
.orga $6286
.section "kernel: use new font 1" overwrite
  phx
  phy
    doTrampolineCallNew1 fetchNewFontCharData
  ply
  plx
  jmp $629B
.ends

.bank metabank_new1 slot std_slot
.section "kernel: use new font 2" free
  fetchNewFontCharData:
    ; convert raw glyph index to table offset
    lda nextPrintCodepointHi.w
;    sta @emphSpCheckInstr+1.w
    sec
    sbc #fontBaseOffset
    tay
    
    ;=====
    ; multiply font table offset by 10 to get src offset in font data
    ;=====
    
    ; multiply by 2, saving result separately for future use
    asl
    sta @fontFinalAdd1+1.w
    ; the input will not be greater than 0x80, so the high byte
    ; from multiplying by 2 will always be zero; we don't need to
    ; bother explicitly computing it here
;    lda $F9.b
;    rol
;    sta @fontFinalAdd2+1.w
    stz $F9.b
    
    ; shift left twice to get (raw * 8)
    asl
    rol $F9.b
    asl
    rol $F9.b
    
    ; add base pointer for font data
    clc
    adc #<fontStd
    sta $F8.b
    lda $F9.b
    adc #>fontStd
    sta $F9.b
    
    ; add (raw * 2) to (raw * 8) to get (raw * 10)
    @fontFinalAdd1:
    lda #$00
    clc
    adc $F8.b
    sta $F8.b
    
;    @fontFinalAdd2:
;    lda #$00
    cla
    adc $F9.b
    sta $F9.b
    
    ; if font emph on, move to alt font
    lda selectedFontId.w
    beq @stdFont
      cmp #fontId_narrow_crammed
      beq @isNarrow
      cmp #fontId_narrow_smallspace
      beq @isNarrow
      cmp #fontId_narrow
      bne @notNarrow
      @isNarrow:
        lda #<(fontNarrow-fontStd)
        clc
        adc $F8.b
        sta $F8.b
        lda #>(fontNarrow-fontStd)
        adc $F9.b
        sta $F9.b
        bra @stdFont
      @notNarrow:
        lda #<(fontNarrower-fontStd)
        clc
        adc $F8.b
        sta $F8.b
        lda #>(fontNarrower-fontStd)
        adc $F9.b
        sta $F9.b
    @stdFont:
    
    ;=====
    ; look up width
    ;=====
    
    jsr getCharWidth_fromPreconvertedY
    sta currentCharWidth.w
;    sta currentCharRawGlyphWidth.w
    
    ;=====
    ; copy character data to dst buffer
    ;=====
    
    ; offset data by:
    ; - 1 row from top of pattern if in 4-line mode with parity 0
    ; - 5 rows from top of pattern if in 4-line mode with parity 1
    ; - 3 rows from top of pattern if in 3-line mode
    lda #(1*2)
    sta @cpxInstr+1.w
    lda fourLineBoxOn.w
    beq +
      lda br4Parity.w
;      lsr
;      bcc +
      bpl @addFillerRows
      @do5RowOffset:
        lda #((1+4)*2)
        sta @cpxInstr+1.w
      bra @addFillerRows
    +:
    
    lda #((1+2)*2)
    sta @cpxInstr+1.w
    
    @addFillerRows:
    clx
    -:
      stz fontCharBufferStart.w,X
      inx
      @cpxInstr:
      cpx #$00
      bne -
    
    cly
    -:
      ; copy left part
      lda ($F8.b),Y
      sta fontCharBufferStart.w,X
      inx
      
      ; blank right part
      stz fontCharBufferStart.w,X
      inx
      
      iny
      cpy #bytesPerRawFontChar
      bne -
    
    ; blank remaining rows of buffer
    -:
      stz fontCharBufferStart.w,X
      inx
      cpx #fontCharBufferSize
      bne -
    
    rts
  
  ; A = original codepoint (not converted to raw font index)
  getCharWidth:
    sec
    sbc #fontBaseOffset
    tay
  getCharWidth_fromPreconvertedY:
    phx
    ; if font emph on, use alt font table
    lda selectedFontId.w
    beq @stdFont
      cmp #fontId_narrow_crammed
      beq @isNarrow
      cmp #fontId_narrow_smallspace
      beq @isNarrow
      cmp #fontId_narrow
      bne @notNarrow
      @isNarrow:
;        lda fontWidthNarrow.w,Y
;        bra @done
        lda fontWidthNarrow.w,Y
        ldx selectedFontId.w
        
        cpx #fontId_narrow_smallspace
        bne +
          ; if a space, reduce width by 1px
          ; (note that final character of string should not be space,
          ; or it'll be double-decremented.
          ; this shouldn't happen, though.)
          cpy #$00
          bne ++
          @decNarrow:
            dea
            bra @finalChecksNarrow
          ++:
          
          ; HACK: if 'T', reduce width by 1px
;          cpy #code_T_preconverted
;          beq @decNarrow
          
          bra @finalChecksNarrow
        +:
        
        cpx #fontId_narrow
        beq +
        ; if crammed
          dea
          bra @doneNarrow
        +:
        
        @finalChecksNarrow:
        
        ; if not crammed
        ; HACK: if next byte is terminator, --width
        ldx nextByteToFetch.w
        cpx #opcode_terminator
        bne +
          dea
        +:
        
        @doneNarrow:
        plx
        rts
      @notNarrow:
        lda fontWidthNarrower.w,Y
        ldx selectedFontId.w
        
        cpx #fontId_narrower_smallspace
        bne +
          ; if a space, reduce width by 1px
          ; (note that final character of string should not be space,
          ; or it'll be double-decremented.
          ; this shouldn't happen, though.)
          cpy #$00
          bne ++
          @decNarrower:
            dea
            bra @finalChecksNarrower
          ++:
          
          ; HACK: if 'T', reduce width by 1px
;          cpy #code_T_preconverted
;          beq @decNarrower
          
          bra @finalChecksNarrower
        +:
        
        cpx #fontId_narrower
        beq +
        ; if crammed
          dea
          bra @doneNarrower
        +:
        
        @finalChecksNarrower:
        
        ; if not crammed
        ; HACK: if next byte is terminator, --width
        ldx nextByteToFetch.w
        cpx #opcode_terminator
        bne ++
          dea
        ++:
        
        @doneNarrower:
        plx
        rts
    @stdFont:
    lda fontWidthStd.w,Y
    @done:
    plx
    rts
.ends

;===================================
; use new char widths
;===================================

.bank metabank_kernel slot std_slot
.orga $64BE
.section "kernel: use new char width 1" overwrite
  jsr addLastCharWidth
.ends

.bank metabank_kernel slot std_slot
.section "kernel: use new char width 2" free
  addLastCharWidth:
    clc
    adc currentCharWidth.w
    rts
.ends

.bank metabank_kernel slot std_slot
.orga $64CA
.section "kernel: use new char width 3" overwrite
  jmp checkForNoCoarseXAdvance
.ends

.bank metabank_kernel slot std_slot
.section "kernel: use new char width 4" free
  checkForNoCoarseXAdvance:
    ; make up work
    lsr
    lsr
    lsr
    
    ; if result zero, skip coarse X position advance
    bne +
      rts
    +:
    jmp $64CD
.ends

;===================================
; new string length calculation
;===================================

.bank metabank_kernel slot std_slot
.section "kernel: new string width 1" free
  ; textPtr = pointer to source string
  ; returns lastStringWidth = width of string
  getStringWidth:
    stz widthChecksAreForWord.w
    @altEntry:
    ; TODO: doing this backup/switch may not be necessary, but let's play it safe
;    phx
      lda freeBankMpr6B.b
      pha
        tma #$40
        sta freeBankMpr6B.b
        doTrampolineCallNew1 getStringWidth_sub
;        tax
      pla
      sta freeBankMpr6B.b
;      txa
;    plx
    rts
  
  lastStringWidth:
    .db $00
  
  ; textPtr = pointer to source string
  ; freeBankMpr6B = bank to be paged in when reading from string
  ; returns lastStringWidth = width of string
/*  getWordWidth:
    lda #$FF
    sta widthChecksAreForWord.w
    bra getStringWidth@altEntry*/
  
  widthChecksAreForWord:
    .db $00
  
  ; textPtr = pointer to source string
  fetchByteWithFreeBankIn:
    tma #$40
    pha
    lda freeBankMpr6B.b
    tam #$40
;      lda freeBankOverride.w
;      beq +
;        tam #$40
;      +:
      
/*      lda (textPtrB.b)
      sta lastFetchedByte.w
    pla
    tam #$40
    jmp incTextPtr*/
      
      lda (textPtrB.b)
      sta lastFetchedByte.w
      pha
        jsr incTextPtr
      pla
      
      ; HACK
      ; fetch next byte (so we can peek at the upcoming input)
      ; ...but not if about to start a dte sequence
      ; (HACK, this works only because we only check nextByteToFetch
      ; if we're actually printing and then only to check for a terminator)
      stz nextByteToFetch.w
      cmp #dteEncodingLowerLimit
      bcc ++
        lda dteSeqActive.w
        beq +
        ++:
          lda (textPtrB.b)
          sta nextByteToFetch.w
      +:
    pla
    tam #$40
    rts
  
  lastFetchedByte:
    .db $00
  nextByteToFetch:
    .db $00
  
;  freeBankOverride:
;    .db $00
    
.ends

.bank metabank_new1 slot std_slot
.section "kernel: new string width 2" free
  ; textPtr = pointer to source string
  ; freeBankMpr6B = bank to be paged in when reading from string
  ; returns lastStringWidth = width of string
  getStringWidth_sub:
    ; save all state variables that may be changed by this operation
    ; TODO: we can probably leave out all of the substring-related stuff;
    ; it will most likely only be needed for the word-boundary detection routine
    jsr saveSubstringMemBlock
    
    lda dteSeqActive.w
    pha
    lda textPtrLoB.b
    pha
    lda textPtrHiB.b
    pha
    lda selectedFontId.w
    pha
    lda lastReadOpOrPrintSubstate.w
    pha
    phx
    phy
      
      stz nonSpaceFound.w
      stz lastStringWidth.w
      @loop:
        jsr fetchNextTextByte
        
        cmp #newOpLowerLimit
        bcc @literal
          cmp #opcode_printNum
          bne +
            ; TODO
            ; for now, pretend all numbers are 5 digits of 6px each
            lda #6*5
            bra @addToWidth
          +:
          ; anything else is considered the end of the string
          bra @done
        @literal:
        
        tax
          lda widthChecksAreForWord.w
          beq @noWordEndChecks
            ; flag non-space as encountered if needed
            cpx #code_space
            beq +
              lda #$FF
              sta nonSpaceFound.w
            +:
            
            lda nonSpaceFound.w
            beq @noWordEndChecks
              ; check if new char is a word divider
              cpx #code_space
              beq @isWordDivider
              bra @noWordEndChecks
              @isWordDivider:
                bra @done
        @noWordEndChecks:
        txa
        
        jsr getCharWidth
        
        @addToWidth:
        clc
;        adc @resultOp+1.w
;        sta @resultOp+1.w
        adc lastStringWidth.w
        sta lastStringWidth.w
        
        bra @loop
    @done:
/*    ; HACK: the final character of ANY narrowed but not crammed string
    ; has width-1, because the rightmost column of each character is blank.
    ldy selectedFontId.w
    ; do not apply for normal font because it'll probably screw something
    ; up, and this is only needed to cram in a few borderline item names
    beq +
      dec lastStringWidth.w
    +:*/
    
    ply
    plx
    pla
    sta lastReadOpOrPrintSubstate.w
    pla
    sta selectedFontId.w
    pla
    sta textPtrHiB.b
    pla
    sta textPtrLoB.b
    pla
    sta dteSeqActive.w
    
    jmp restoreSubstringMemBlock
  
  saveSubstringMemBlock:
    tii stringSubstringMemBlockStart,stringSubstringMemBackup,stringSubstringMemBlockSize
    tii globalRedirectBuffer,globalRedirectBufferBackup,globalRedirectBufferSize
    rts
  
  restoreSubstringMemBlock:
    tii stringSubstringMemBackup,stringSubstringMemBlockStart,stringSubstringMemBlockSize
    tii globalRedirectBufferBackup,globalRedirectBuffer,globalRedirectBufferSize
    rts
  
  stringSubstringMemBackup:
    .ds stringSubstringMemBlockSize,$00
  globalRedirectBufferBackup:
    .ds globalRedirectBufferSize,$00
  
  nonSpaceFound:
    .db $00
  
  readNewTextPtr:
    jsr fetchByteWithFreeBankIn
    lda lastFetchedByte.w
    pha
      jsr fetchByteWithFreeBankIn
      lda lastFetchedByte.w
      sta textPtrHiB.b
    pla
    sta textPtrLoB.b
    rts
  
  ; textPtr = pointer to source string
  ; freeBankMpr6B = bank to be paged in when reading from string
  ; returns A = next byte of string
  ; 
  ; ops are handled as follows:
  ; - local jumps (our new 0xEE op) are silently followed and omitted from output
  ; - substring print op: silently followed and omitted; 
  ;   terminators for substrings are similarly automatically followed back to
  ;   the parent string
  ; - number print op: ??? TODO
  ; - anything else is returned as normal
  fetchNextTextByte:
    @loop:
      jsr fetchByteWithFreeBankIn
      
      lda lastFetchedByte.w
      cmp #newOpLowerLimit
      bcc @literal
        cmp #opcode_localRedirect
        bne +
          bsr readNewTextPtr
          bra @loop
        +:
        cmp #opcode_globalRedirect
        bne +
          bsr readNewTextPtr
          jsr doGlobalTextRedirect@altEntry
          bra @loop
        +:
        cmp #opcode_printString
        bne +
          ; call standard substring handler from old code
          jsr $6053
          bra @loop
        +:
        cmp #opcode_terminator
        bne @done
          ; if no substring active and we encounter a terminator, we're done
          lda substringPrintActive.w
          bne +
            lda #opcode_terminator
            bra @done
          +:
            ; substring active: deactivate
            ; back up this state var, which is changed by the called code
            ; FIXME: is this really what we want?
;            lda lastReadOpOrPrintSubstate.w
;            pha
              ; call regular terminator handler, which takes the desired action
              ; for us
              jsr $60A1
;            pla
;            sta lastReadOpOrPrintSubstate.w
            bra @loop
      @literal:
      
      ; handle dte
      cmp #dteEncodingLowerLimit
      bcc +
;          jsr handle8bitTextLiteral_sub
        jsr doDteLookup
        pha
          jsr handle8bitTextLiteral_sub@altEntry
        pla
      +:
    
    @done:
    rts
  
  ; NOTE: textPtr had better have at least one space to advance past!
/*  advanceTextPtrPastSpaces:
    @loop:
      jsr fetchNextTextByte
      cmp #code_space
      beq @loop
    rts*/
  
  ; returns A nonzero if autowrap occurred
  checkAndHandleAutoWordWrap:
    ; we have detected an upcoming space in the text;
    ; determine if next word, including space(s), will fit on the line
    jsr decTextPtr
    lda #$FF
    sta widthChecksAreForWord.w
    jsr getStringWidth_sub
    
    ; get current pixel position in box
    lda textCoarseX.w
    asl
    asl
    asl
    clc
    adc textFineX.w
    ; add next word width
    clc
    adc lastStringWidth.w
    ; if > 255, will not fit
    bcs @overflow
      ; otherwise, if less than max x-pos, will fit
      cmp #maxTextX+1
      bcc @doneNoBreak
    @overflow:
    
    ; advance src past spaces
;    jsr advanceTextPtrPastSpaces
    ; never mind, too damn complicated for what we're actually going to be
    ; dealing with.
    ; just skip the next character and assume that takes care of it.
    jsr fetchNextTextByte
    
    ; shift printing pos to next line
;    jsr doLinebreak
    ; NOTE: we skip all the normal y-overflow checks here;
    ; do not use autowrap if text is not guaranteed to fit in one box
;    stz textFineX.w
;    stz textCoarseX.w
;    inc textCoarseY.w
;    inc textCoarseY.w
    
    @done:
    lda #$FF
    rts
    @doneNoBreak:
    lda #$00
    rts
.ends

;===================================
; 
;===================================

/*.bank metabank_kernel slot std_slot
.orga $85B7
.section "kernel: window bottom 1" overwrite
  jsr drawWindowTopBottom
.ends

.bank metabank_kernel slot std_slot
.section "kernel: window bottom 2" free
  drawWindowTopBottom:
    ; drawWindowTop
    jsr $8B0E
    ; drawWindowBottom
    jmp $8B80
.ends

.bank metabank_kernel slot std_slot
.orga $8604
.section "kernel: window bottom 3" overwrite
  nop
  nop
  nop
.ends*/

;===================================
; use new string width calculations:
; externally referenced menu string
;===================================

.bank metabank_kernel slot std_slot
.orga $8702
.section "kernel: use new string width for external menu strings 1" overwrite
  ; back up textPtr
  lda textPtrLoB.b
  pha
  lda textPtrHiB.b
  pha
    jsr computeExternalMenuStrSizeInfo
  pla
  sta textPtrHiB.b
  pla
  sta textPtrLoB.b
  nop
  nop
.ends

.bank metabank_kernel slot std_slot
.section "kernel: use new string width for external menu strings 2" free
  ; computes string width with automatic narrowing for print-to-fit,
  ; and leaves selectedFontId set to the ID of the font which should
  ; be used to print the string
  getStringWidthWithNarrowing:
    @check:
    jsr getStringWidth
    
    ; check if print-to-fit on
    lda printToFitOn.w
    beq @noNarrowing
      lda lastStringWidth.w
      cmp printToFit_maxW.w
      beq @noNarrowing
      bcc @noNarrowing
      
      ; can't narrow further if on last font
      lda selectedFontId.w
      cmp #maxFontId
      beq @noNarrowing
      
        ; didn't fit: try next font
        inc selectedFontId.w
        bra @check
    @noNarrowing:
    rts
  
  computeExternalMenuStrSizeInfo:
    ; copy ($CD) to textStrPtr
    ldy #1
    lda ($CD.b)
    sta textPtrLoB.b
    lda ($CD.b),Y
    sta textPtrHiB.b
    
    jsr getStringWidthWithNarrowing
    
;    doTrampolineCallNew1 computeExternalMenuStrSizeInfo_sub
    ; convert pixel width to tile count
    ; formula: ((pixelW + 7) / 8)
    lda lastStringWidth.w
    clc
    adc #$7
    lsr
    lsr
    lsr
    
    ; a tile width of zero should be considered as 1
    ; (callers do not account for the possibility of a zero width)
    bne +
      ina
    +:
    
    ; save tile count
    sta strTileCount.w
    
    rts
.ends

.bank metabank_kernel slot std_slot
.orga $8722
.section "kernel: use new string width for external menu strings 3" overwrite
  ; correctly reload textPtr to account for above changes
  phy
    ldy #1
    lda ($CD.b)
    sta textPtrLoB.b
    lda ($CD.b),Y
    sta textPtrHiB.b
  ply
  nop
.ends

;===================================
; account for possibility of an
; embedded menu string taking up
; 1 tile or less
;===================================

.bank metabank_kernel slot std_slot
.orga $8732
.section "kernel: allow 1-tile embedded strings 1" overwrite
  jmp doShortEmbeddedStringCheck
.ends

.bank metabank_kernel slot std_slot
.section "kernel: allow 1-tile embedded strings 2" free
  doShortEmbeddedStringCheck:
    ; make up work
    ldx strTileCount.w
    
    ; check tile size of string
    ; if 0, do not advance src
/*    cpx #0
    bne +:
    @noAdvance:
      jmp $873C
    +:
    ; if 1, do not decrement before 
    cpx #1
    beq +
      jmp $8736
    +:*/
    
    ; if <= 1, do not advance src
    cpx #2
    bcs +
      jmp $873C
    +:
    jmp $8735
    
.ends

;===================================
; new item table
;===================================

/*.bank metabank_kernel slot std_slot
.orga $6FF7
.section "kernel: new item table 1" overwrite
  lda #<itemTable
  sta textPtrLoB.b
  lda #>itemTable
  sta textPtrHiB.b
.ends

.bank metabank_kernel slot std_slot
.section "kernel: new item table 2" free
  itemTable:
    .incbin "out/script/items/itemtable.bin"
.ends*/

.include "out/script/items/itemlist.inc"

.bank metabank_kernel slot std_slot
.orga $6FF4
.section "kernel: use new item table 1" SIZE $28 overwrite
  getItemNameByIndex:
    ; multiply index by 3, add itemTableIndex,
    ; and store result to textPtr
    
    sta @addInstr+1.w
    stz @hiByteInstr+1.w
    
    ; *2
    asl
    rol @hiByteInstr+1.w
    
    ; *3
    clc
    @addInstr:
    adc #$00
    pha
      cla
      adc @hiByteInstr+1.w
      sta @hiByteInstr+1.w
    pla
    
    clc
    adc #<itemTableIndex
    sta textPtrLoB.b
    
    lda #>itemTableIndex
    @hiByteInstr:
    adc #$00
    sta textPtrHiB.b
    
    rts
.ends

;===================================
; new yes/no prompt
;===================================

.bank metabank_kernel slot std_slot
.orga $5F5D
;.section "kernel: new yes/no prompt 1" SIZE $37 overwrite
.section "kernel: new yes/no prompt 1" SIZE $44 overwrite
  cly
  lda #$09
  sta textCoarseX.w
  lda #$04
  sta textFineX.w
  @loop:
    lda newYesNoPrompt.w,Y
    bne @notNull
      ; jump to hardcoded new position
      lda #15
      sta textCoarseX.w
      lda #$04
      sta textFineX.w
      bra @loopEnd
    @notNull:
    cmp #$FF
    beq @done
      sta nextPrintCodepointHi.w
      jsr printToBox
      jsr advancePrintPos
      @loopEnd:
      iny
      bra @loop
  @done:
  lda #$02
  sta currentPrintMode.w
  stz $272E.w
  stz $33F3.w
  stz $2731.w
  rts 
  
  newYesNoPrompt:
    .incbin "out/script/misc/yesno_prompt.bin"
.ends

;.bank metabank_kernel slot std_slot
;.orga $5F94
;.section "kernel: new yes/no prompt 2" SIZE $D overwrite
;  newYesNoPrompt:
;    .incbin "out/script/misc/yesno_prompt.bin"
;.ends

;===================================
; adjust digit conversion for new
; font
;===================================

.bank metabank_kernel slot std_slot
.orga $6039
.section "kernel: new digit conversion 1" overwrite
  ; $15 = target digit
  ; convert to font code and set up as print value
  lda #fontDigitBase
  clc
  adc $15.b
  sta nextPrintCodepointHi.w
  jmp $6046
.ends

;===================================
; if a digit was not printed, don't
; pause as if one was
;===================================

.bank metabank_kernel slot std_slot
.orga $5FDA
.section "kernel: no unprinted digit pause 1" overwrite
  ; needed as branch target
  textOpPrintNum_handler:
    lda $33F7.w
.ends

; to avoid printing leading zeroes, the original game returns carry set,
; indicating nothing should be printed, if it hasn't reached a nonzero digit yet.
; however, this still triggers the normal waiting period between characters,
; meaning there's a pause before any number which isn't 5 digits long
; that grows increasingly longer the shorter the value.
; we just loop the routine back to the start to prevent this.
.bank metabank_kernel slot std_slot
.orga $602F
.section "kernel: no unprinted digit pause 2" overwrite
  beq textOpPrintNum_handler
.ends

;===================================
; remove old tile-based auto-wrap
;===================================

.bank metabank_kernel slot std_slot
.orga $64D2
.section "kernel: remove old auto-wrap 1" overwrite
  jmp $64E2
.ends

;===================================
; double initial text speed
;===================================

.bank metabank_kernel slot std_slot
.orga $48B5
.section "kernel: double initial text speed 1" overwrite
;  lda #$03
  lda #$01
.ends

;===================================
; double occasionally-used override
; text speed (for printStringGeneric)
;===================================

.bank metabank_kernel slot std_slot
.orga $6C74
.section "kernel: double override text speed 1" overwrite
;  lda #$01
  lda #$00
.ends

;===================================
; new text speed load
;===================================

.bank metabank_kernel slot std_slot
.section "kernel: new text speed load 1" free
  getTextSpeed:
    lda textSpeedSetting.w
    bpl +
      ; if top bit set, assume "turbo"
      and #$7F
    +:
    sta @resultCmd+1.w
    
    ; double text speed if autobattle on
    ; just to be really goddamn annoying, the autobattle flag is not cleared
    ; at the end of battle, so we must also specifically check if we are in
    ; battle mode
    lda gameModeB.b
    cmp #$02
    bne +
      lda autoBattleOn.w
      beq +
        lsr @resultCmd+1.w
    +:
    
    @resultCmd:
    lda #$00
    rts
.ends

.bank metabank_kernel slot std_slot
.orga $5D69
.section "kernel: new text speed load 2" overwrite
  jsr getTextSpeed
.ends

;===================================
; add instant-print mode
;===================================

.bank metabank_kernel slot std_slot
.orga $5B99
.section "kernel: instant print 1" overwrite
  doTrampolineCallNew1 doInstantPrintCheck
  jmp $5BA7
.ends

.bank metabank_new1 slot std_slot
.section "kernel: instant print 2" free
  doInstantPrintCheck:
    ; clear frame skipped flag
    stz lastFrameWasSkipped.w
    
    ; text speed setting must be 0 (the translation's maximum)
;    lda textSpeedSetting.w

;    jsr getTextSpeed
    ; don't use getTextSpeed; we want to know the real speed,
    ; unadjusted for autobattle
    lda textSpeedSetting.w
    bmi +
      ; if not turbo and speed is not "fast", do wait
      bne @doWait
    +:
    
    ; check if force wait on first frame of display needed
    ; (needed to make sure character portraits show up at the start
    ; of the box rather than after the text finishes printing)
    lda initialBoxWaitNotNeeded.w
    beq @doWait
    
    ; check if in a substate which require an idle for correct behavior
    ; TODO: any other states we need to cover?
    lda lastReadOpOrPrintSubstate.w
    cmp #$02
    beq @doWait
    cmp #$03
    beq @doWait
    ; ?
    cmp #opcode_linebreak
    beq @doWait
    
    ; check if turbo mode on
    tst #$80,textSpeedSetting.w
    bne @skip
/*    beq @noTurboMode
      ; ?
      lda lastReadOpOrPrintSubstate.w
      cmp #opcode_linebreak
      beq @doWait
      bra @skip*/
    @noTurboMode:
    
    ; if we're here, text speed is "fast".
    ; toggle parity flag and only do skip check if it's nonzero,
    ; limiting text to two characters per frame.
    lda @parityFlag.w
    eor #$01
    sta @parityFlag.w
    beq @doWait
    
    ; if in battle mode and autobattle is on, ignore usual button check
    ; (parity check will handle throttling)
    lda gameModeB.b
    cmp #$02
    bne +
      lda autoBattleOn.w
      bne @skip
    +:
    
    ; check if fast-forward button pressed
    tst #$01,JOY
    beq @doWait
    
    @skip:
    lda #$FF
    sta lastFrameWasSkipped.w
    
    ; skipping the frame wait means input will not be updated.
    ; however, we want the fast-forward to stop if the user releases
    ; the button, so we update manually here.
    ; TODO: any problems here due to interrupts, etc.?
    jsr EX_JOYSNS
    ; if autobattle is on, set JOY to low bit of frame counter
    lda gameModeB.b
    cmp #$02
    bne +
      lda autoBattleOn.w
      beq +
        lda $0E.b
        and #$01
        sta JOY.w
    +:
    bra @end
    
    @doWait:
    jsr doStdFrameEnd
    lda #$FF
    sta initialBoxWaitNotNeeded.w
    
    @end:
    lda $3403.w
    bne +
      jsr $5BB8
      jsr $5D2A
    +:
    rts
    
    @parityFlag:
      .db $00
    
  lastFrameWasSkipped:
    .db $00

  doSpriteDrawSkipCheck:
    ; if frame was skipped, do not update and redraw sprites
    lda lastFrameWasSkipped.w
    bne @done
      ; update and redraw sprites
      ; clearSpriteTable
      jsr $44A3
      ; ???
      jsr $5C56
      ; updateAndSendObjectSprites
      jsr $7596
    @done:
    ; clear frame skipped flag
    stz lastFrameWasSkipped.w
    rts
  
  doExtraSpriteDrawInit:
    ; flag first-frame box wait as needed
    stz initialBoxWaitNotNeeded.w
    ; clear any holdover frame skipped flag from previous message
    stz lastFrameWasSkipped.w
    rts
    
.ends

;===================================
; freeze animations during
; insta-print
;===================================

.bank metabank_kernel slot std_slot
.orga $5B85
.section "kernel: instant print animation freeze 1" overwrite
  doTrampolineCallNew1 doSpriteDrawSkipCheck
  nop
  nop
  nop
.ends

;===================================
; ensure portraits get displayed
; even if instant printing on
;===================================

; the game only refreshes sprites when the normal wait-for-frame-end
; routine is called, and if instant printing is occurring at the start
; of the box, the portrait sprite overlay never gets sent.
; we can solve this by forcing one immediate frame wait right after
; the portrait is generated.

.bank metabank_kernel slot std_slot
.orga $5B82
.section "kernel: force portrait refresh 1" overwrite
  jsr doPortraitRefresh
.ends

.bank metabank_kernel slot std_slot
.section "kernel: force portrait refresh 2" free
  doPortraitRefresh:
    doTrampolineCallNew1 doExtraSpriteDrawInit
    ; make up work
    jmp $4C4C
  
  initialBoxWaitNotNeeded:
    .db $FF
.ends

;===================================
; remove buttons-released check
; from text advance routine
;===================================

.bank metabank_kernel slot std_slot
.orga $5BEE
.section "kernel: no text button release 1" overwrite
;  jmp doExtraButtonReleaseCheck
  jmp $5C04
.ends

/*.bank metabank_kernel slot std_slot
.section "kernel: no text button release 2" free
  doExtraButtonReleaseCheck:
    ; 
.ends*/

.bank metabank_kernel slot std_slot
.orga $5BD5
.section "kernel: no text button release 2" overwrite
  tst #$F3,JOYTRG
.ends

;===================================
; allow adpcm clips to be skipped
; by pressing button 1
;===================================

.bank metabank_kernel slot std_slot
.orga $621D
.section "kernel: adpcm skip 1" overwrite
  doTrampolineCallNew1 doAdpcmSkipCheck
  rts
.ends

.bank metabank_new1 slot std_slot
.section "kernel: adpcm skip 2" free
  doAdpcmSkipCheck:
    ; make up work
    @loop:
      jsr checkIfBoxEndAllowedYet
      bcc @done
        jsr doShortFrameIdle
        lda JOY.w
        and #$02
        and allowedSkipButtons.w
        bne @skipOn
        
        ; new check for button 1 skip
        lda JOYTRG.w
        and #$01
;        bne @cancel
        beq +
          jsr AD_STOP
        +:
        bra @loop
    @skipOn:
    lda #$01
    sta forceNoTextDelayOn
;    @cancel:
    jsr stopAdpcmIfNeeded
    @done:
    rts
.ends

;===================================
; automatic line filling for menus
;===================================

.bank metabank_kernel slot std_slot
;.orga $85EE
.orga $85D8
.section "kernel: menu line autofill 1" overwrite
  jsr autoFillMenuLine
.ends

.bank metabank_kernel slot std_slot
.section "kernel: menu line autofill 2" free
  autoFillMenuLine:
    lda menuBaseXB.b
    dea
    clc
    adc currentWindowW.w
    and #$3F
    sta @checkInstr+1.w
    lda #$20
    sta lastWindowByte.w
    -:
      lda menuCurrentTargetXB.b
      @checkInstr:
      cmp #$00
      beq @done
      jsr printFetchedCharOrSubContent
      bra -
    
    @done:
    ; make up work
;    lda menuCurrentTargetYB.b
;    ina
    jmp drawWindowRight
.ends

;===================================
; modify computeSjisStringSizeInfo
; to handle new string format
;===================================

.bank metabank_kernel slot std_slot
.orga $8835
.section "kernel: new computeSjisStringSizeInfo 1" overwrite
  jmp doNewComputeSjisStringSizeInfo
.ends

.bank metabank_kernel slot std_slot
.section "kernel: new computeSjisStringSizeInfo 2" free
  ; $CF-F0 = src
  doNewComputeSjisStringSizeInfo:
    ; copy src to textPtr
    lda $CF.b
    sta textPtrLoB.b
    lda $D0.b
    sta textPtrHiB.b
    ; get pixel width
    jsr getStringWidth
    
    ; convert pixel width to tile count
    ; (width assumed nonzero)
    lda lastStringWidth.w
    clc
    adc #7
    lsr
    lsr
    lsr
    ; save result
    sta strTileCount.w
    
    ; search for string end and set strByteSize accordingly
    phy
      ldy #$FF
      -:
        iny
        lda (textPtrB.b),Y
        cmp #newOpLowerLimit
        bcc -
      tya
      sta strByteSize.w
    ply
    
    rts
.ends

;===================================
; new embedded string printing
; for menus
;===================================

.bank metabank_kernel slot std_slot
.orga $8641
.section "kernel: menu embedded strings 1" overwrite
  lda lastWindowByte.w
  cmp #menuOpcode_embeddedString
  beq +
    ; branch to normal literal handler
    jmp $8651
    nop
  +:
.ends

.bank metabank_kernel slot std_slot
.orga $8795
.section "kernel: menu embedded strings 2" overwrite
  jmp doExtraEmbeddedStringLogic
.ends

.bank metabank_kernel slot std_slot
.orga $877D
.section "kernel: menu embedded strings 3" overwrite
  jsr doNewEmbeddedStringCopy
.ends

.bank metabank_kernel slot std_slot
.section "kernel: menu embedded strings 4" free
  doNewEmbeddedStringCopy:
    ; skip embed code
    jsr fetchFromRefedSrc
    ; make up work
    jmp $8798
.ends

.bank metabank_kernel slot std_slot
.section "kernel: menu embedded strings 5" free
  doExtraEmbeddedStringLogic:
    ; make up work
    sta menuCurrentTargetXB.b
    
    ; peek at next script char
    phy
      ldy menuCurrentStringIndexB.b
      lda ($2B.b),Y
    ply
    
    ; if an embed command (signalling end of string),
    ; skip it and advance src by number of tiles in string
    cmp #menuOpcode_embeddedString
    beq +
      ; otherwise, it SHOULD be a linebreak.
      ; leave it to be found on the next loop.
      rts
    +:
    
    ; skip
;    jsr fetchFromRefedSrc
    
    ; skip needed number of bytes
    phx
      ldx strTileCount.w
;      dex
      inx
      -:
        jsr fetchFromRefedSrc
        dex
        bne -
    plx
    
    @done:
    rts
.ends

;===================================
; repeat command for menus
;===================================

; acknowledge when computing dimensions if on first line

.bank metabank_kernel slot std_slot
.orga $8AF4
.section "kernel: menu repeat command 1" overwrite
  jmp doRepeatCmdCheck
.ends

.bank metabank_kernel slot std_slot
.orga $8AE9
.section "kernel: menu repeat command 1b" overwrite
  computeWindowDimensions_loopStart:
    lda ($CF.b),Y
.ends

.bank metabank_kernel slot std_slot
.section "kernel: menu repeat command 2" free
  ; check if the byte we just fetched to A is a repeat command
  doRepeatCmdCheck:
    ; do nothing if not on first line
    pha
      lda $3C09.w
      beq @process
        pla
        jmp computeWindowDimensions_loopStart
    @process:
    pla
    
    cmp #menuOpcode_repeat
    beq @isRept
;      lda #$01
;      bra @done
      inc currentWindowW.w
      jmp computeWindowDimensions_loopStart
    @isRept:
      ; fetch next byte
      lda ($00CF),Y
      iny
      bne +
        inc $D0.b
      +:
    @done:
    clc
    adc currentWindowW.w
    sta currentWindowW.w
    jmp computeWindowDimensions_loopStart
.ends

.bank metabank_kernel slot std_slot
.orga $8651
.section "kernel: menu repeat command 3" overwrite
  jmp doExtraMenuOpsCheck
.ends

.bank metabank_kernel slot std_slot
.section "kernel: menu repeat command 4" free
  doExtraMenuOpsCheck:
    ; make up work
    lda lastWindowByte.w
    
    cmp #menuOpcode_repeat
    beq @handleRepeat
    cmp #menuOpcode_narrowItem
    beq @handleNarrowItem
    cmp #menuOpcode_narrowMapName
    beq @handleNarrowMapName
      jmp $8654
    
    @handleRepeat:
    ; TODO
    ; actually, we probably don't even need this outside of specifying
    ; the window dimensions on the first line, so maybe just ignoring it is fine
    jsr fetchFromRefedSrc
    jmp $865D
;    jmp $8628
    
    @handleNarrowItem:
      lda #maxNarrowItemNameW
      bra @finishNarrow
    @handleNarrowMapName:
      lda #maxNarrowMapNameW
      ; !!! drop through !!!
    @finishNarrow:
    sta printToFit_maxW.w
    lda #$FF
    sta printToFitOn.w
      lda selectedFontId.w
      pha
        ; do normal external string print
        jsr $86FA
      pla
      sta selectedFontId.w
    stz printToFitOn.w
;    jmp $8628
    jmp $865D
  
  printToFitOn:
    .db $00
  printToFit_maxW:
    .db printToFit_defaultMaxW
.ends

;===================================
; implement printToFit for
; printItemNameByIndex
;===================================

.bank metabank_kernel slot std_slot
.orga $6FF1
.section "kernel: printToFit for printItemNameByIndex 1" overwrite
  jmp doPrintItemNameFitCheck
.ends

.bank metabank_kernel slot std_slot
.section "kernel: printToFit for printItemNameByIndex 2" free
  doPrintItemNameFitCheck:
    lda #$FF
    sta printToFitOn.w
    lda selectedFontId.w
    pha
      lda #maxNarrowItemNameW
      sta printToFit_maxW.w
      ; make up work
      jsr printStringRawAltEntry
    pla
    sta selectedFontId.w
    stz printToFitOn.w
    rts
.ends

;===================================
; implement printToFit for
; printStringRaw
;===================================

.bank metabank_kernel slot std_slot
.orga $6521
.section "kernel: printToFit for printStringRaw 1" overwrite
  jmp doPrintStringRawFitCheck
.ends

.bank metabank_kernel slot std_slot
.section "kernel: printToFit for printStringRaw 2" free
  doPrintStringRawFitCheck:
    ; make up work
    jsr switchMpr6To85Bank
    
    ; narrow as needed
    lda printToFitOn.w
    beq +
      jsr getStringWidthWithNarrowing
    +:
    
    ; make up work
    jmp $6524
.ends

;===================================
; generic item printToFit
;===================================

.bank metabank_kernel slot std_slot
.section "kernel: printToFit item generic 1" free
  printStringRawWithItemNarrowing:
    lda #$FF
    sta printToFitOn.w
    lda selectedFontId.w
    pha
      lda #maxNarrowItemNameW
      sta printToFit_maxW.w
      ; make up work (printStringRawAltEntry)
      jsr printStringRaw
    pla
    sta selectedFontId.w
    stz printToFitOn.w
    rts
.ends

;===================================
; transfer only 8px of font data
; per character send instead of
; 16px, increasing speed
;===================================

.bank metabank_kernel slot std_slot
.orga $6425
.section "kernel: 8px character vram send 1" overwrite
  rts
.ends

;===================================
; character names
;===================================

.bank metabank_kernel slot std_slot
.orga $5893
.section "kernel: character names 1" SIZE $B overwrite
  .incbin "out/script/menu/kernel/str-0x1893.bin"
.ends

.bank metabank_kernel slot std_slot
.orga $589E
.section "kernel: character names 2" SIZE $B overwrite
  .incbin "out/script/menu/kernel/str-0x189E.bin"
.ends

.bank metabank_kernel slot std_slot
.orga $58A9
.section "kernel: character names 3" SIZE $B overwrite
  .incbin "out/script/menu/kernel/str-0x18A9.bin"
.ends

.bank metabank_kernel slot std_slot
.orga $58B4
.section "kernel: character names 4" SIZE $B overwrite
  .incbin "out/script/menu/kernel/str-0x18B4.bin"
.ends

.bank metabank_kernel slot std_slot
.orga $58BF
.section "kernel: character names 5" SIZE $B overwrite
  .incbin "out/script/menu/kernel/str-0x18BF.bin"
.ends

.bank metabank_kernel slot std_slot
.orga $58CA
.section "kernel: character names 6" SIZE $B overwrite
  .incbin "out/script/menu/kernel/str-0x18CA.bin"
.ends

;===================================
; orochimaru name patches
;===================================

.bank metabank_kernel slot std_slot
.section "kernel: orochimaru name patches 1" free
  orochimaruNameMenuPatch_normalBottom:
    .incbin "out/script/menu/orochimaru_patch/rawstr-0xD9E.bin"
  orochimaruNameMenuPatch_normalTop:
    .incbin "out/script/menu/orochimaru_patch/rawstr-0xDA8.bin"
  orochimaruNameMenuPatch_kageBottom:
    .incbin "out/script/menu/orochimaru_patch/rawstr-0xDA3.bin"
  orochimaruNameMenuPatch_kageTop:
    .incbin "out/script/menu/orochimaru_patch/rawstr-0xDA9.bin"
.ends

.bank metabank_kernel slot std_slot
.orga $4D89
.section "kernel: orochimaru name patches 2" overwrite
  ; string pointer table
  ; bottom row
  .dw orochimaruNameMenuPatch_normalBottom,orochimaruNameMenuPatch_kageBottom
  ; generic text (no need to change)
  .dw $C5B7,$C5C7
  ; top row
  .dw orochimaruNameMenuPatch_normalTop,orochimaruNameMenuPatch_kageTop
.ends

.bank metabank_kernel slot std_slot
.orga $4D9B
.section "kernel: orochimaru name patches 3" overwrite
  ; string size table
  ; bottom row
;  .db 5
  .db 6
  ; generic text (copy 3 bytes -- redirect command only)
;  .db 9
  .db 3
  ; top row
  ; (doesn't matter for translation)
  .db 1
.ends

/*.bank metabank_kernel slot std_slot
.orga $4D80
.section "kernel: orochimaru name patches 4" overwrite
  ; skip final iteration of loop so top row isn't overwritten
;  cpy #$03
  cpy #$02
.ends*/

;===================================
; four-line box mode
;===================================

; make reset br4 state when box cleared
.bank metabank_kernel slot std_slot
.orga $62DB
.section "kernel: 4-line box mode 1" overwrite
  jmp doBoxClearBr4Reset
.ends

.bank metabank_kernel slot std_slot
.section "kernel: 4-line box mode 2" free
  doBoxClearBr4Reset:
    ; make up work
    stz $27D3.w
    
    stz br4Parity.w
    stz fourLineBoxOn.w
    rts
  
  br4Parity:
    .db $00
  fourLineBoxOn:
    .db $00
.ends

;==============================================================================
; overw
;==============================================================================

; old status menu lower
.unbackground metabank_overw*sizeOfMetabank+$BC2E metabank_overw*sizeOfMetabank+$BD51
; end of bank
.unbackground metabank_overw*sizeOfMetabank+$BF78 metabank_overw*sizeOfMetabank+$BFFF

;===================================
; double text speed
;===================================

.bank metabank_overw slot std_slot
.orga $BE11
.section "overw: double text speed" overwrite
;  .db $07,$03,$01
  .db $03,$01,$00
.ends

;===================================
; new main menu
;===================================

.bank metabank_overw slot std_slot
.orga $B712
.section "overw: new main menu 1" SIZE $69 overwrite
  .incbin "out/script/menu/overw/str-0x2712.bin"
;  .dw $b77b
;  .dw $bd52
;  .dw $b801
;  .dw $b915
;  .dw $b78d
;  .dw $b781
;  .dw $b943
;  .dw $bd84
.ends

.bank metabank_overw slot std_slot
.orga $B6F5
.section "overw: new main menu 2" overwrite
  lda #$14-$4
  sta menuBaseXB.b
  lda #$08
  sta menuBaseYB.b
.ends

;===================================
; new use/give menu
;===================================

.bank metabank_overw slot std_slot
.orga $B7B2
.section "overw: new use/give menu 1" SIZE $19 overwrite
  .incbin "out/script/menu/overw/str-0x27B2.bin"
.ends

.bank metabank_overw slot std_slot
.orga $B79E
.section "overw: new use/give menu 2" overwrite
  lda #$14-$4
  sta menuBaseXB.b
  lda #$08
  sta menuBaseYB.b
.ends

;===================================
; new use/give/drop menu
;===================================

.bank metabank_overw slot std_slot
.orga $B829
.section "overw: new use/give/drop menu 1" SIZE $25 overwrite
  .incbin "out/script/menu/overw/str-0x2829.bin"
.ends

.bank metabank_overw slot std_slot
.orga $B815
.section "overw: new use/give/drop menu 2" overwrite
  lda #$14-$4
  sta menuBaseXB.b
  lda #$08
  sta menuBaseYB.b
.ends

;===================================
; new settings menu
;===================================

.bank metabank_overw slot std_slot
.orga $BD9C
.section "overw: new settings menu 1" SIZE $51 overwrite
  .incbin "out/script/menu/overw/str-0x2D9C.bin"
.ends

.bank metabank_overw slot std_slot
.orga $BD84
.section "overw: new settings menu 2" overwrite
  lda #$00
  sta menuBaseXB.b
  lda #$08
  sta menuBaseYB.b
.ends

;===================================
; new text speed menu
;===================================

/*.bank metabank_overw slot std_slot
.orga $BE14
.section "overw: new text speed menu 1" SIZE $25 overwrite
  .incbin "out/script/menu/overw/str-0x2E14.bin"
.ends*/

; move to kernel for expansion
.bank metabank_kernel slot std_slot
.section "overw: new text speed menu 1" free
  newTextSpeedMenu:
    .incbin "out/script/menu/overw/str-0x2E14.bin"
    ; if new "turbo" option selected
    .dw turboSelected
  
  turboSelected:
    lda #3
    sta $6A.b
    rts
.ends

.bank metabank_overw slot std_slot
.orga $BDF2
.section "overw: new text speed menu 2" overwrite
  lda #$00
  sta menuBaseXB.b
  lda #$10
  sta menuBaseYB.b
  lda #<newTextSpeedMenu
  sta $2B.b
  lda #>newTextSpeedMenu
  sta $2C.b
  jmp runNewTextSpeedMenu
.ends

.bank metabank_overw slot std_slot
.section "overw: new text speed menu 3" free
  runNewTextSpeedMenu:
    ; make up work
    jsr drawAndRunMenu
    ldy $6A.b
    cpy #3
    beq +
      jmp $BE05
    +:
    ; "turbo" setting
    lda #$80
    jmp $BE0C
.ends

;===================================
; new on/off menu
;===================================

.bank metabank_overw slot std_slot
.orga $BE5F
.section "overw: new on/off menu 1" SIZE $19 overwrite
  .incbin "out/script/menu/overw/str-0x2E5F.bin"
.ends

.bank metabank_overw slot std_slot
.orga $BE3E
.section "overw: new on/off menu 2" overwrite
  lda #$00
  sta menuBaseXB.b
  lda #$12
  sta menuBaseYB.b
.ends

;===================================
; new walk type menu
;===================================

.bank metabank_overw slot std_slot
.orga $BE97
.section "overw: new walk type menu 1" SIZE $29 overwrite
  .incbin "out/script/menu/overw/str-0x2E97.bin"
.ends

.bank metabank_overw slot std_slot
.orga $BE7C
.section "overw: new walk type menu 2" overwrite
  lda #$00
  sta menuBaseXB.b
  lda #$0E
  sta menuBaseYB.b
.ends

;===================================
; new status menu lower
;===================================

.bank metabank_overw slot std_slot
.section "overw: new status menu lower 1" free
  overw_statMenuLowerComponent1:
    .incbin "out/script/menu/overw/str-0x2C2E.bin"
.ends

.bank metabank_overw slot std_slot
.section "overw: new status menu lower 2" free
  overw_statMenuLowerComponent2:
    .incbin "out/script/menu/overw/str-0x2C53.bin"
.ends

.bank metabank_overw slot std_slot
.section "overw: new status menu lower 3" free
  overw_statMenuLowerComponent3:
    .incbin "out/script/menu/overw/str-0x2C5C.bin"
.ends

.bank metabank_overw slot std_slot
.section "overw: new status menu lower 4" free
  overw_statMenuLowerComponent4:
    .incbin "out/script/menu/overw/str-0x2C65.bin"
.ends

.bank metabank_overw slot std_slot
.orga $BBA4
.section "overw: new status menu lower 5" overwrite
  .dw overw_statMenuLowerComponent1
.ends

.bank metabank_overw slot std_slot
.orga $BBB3
.section "overw: new status menu lower 6" overwrite
  .dw overw_statMenuLowerComponent2
.ends

.bank metabank_overw slot std_slot
.orga $BBAD
.section "overw: new status menu lower 7" overwrite
  .dw overw_statMenuLowerComponent3
.ends

.bank metabank_overw slot std_slot
.orga $BBB6
.section "overw: new status menu lower 8" overwrite
  .dw overw_statMenuLowerComponent4
.ends

.bank metabank_overw slot std_slot
.orga $B9AA
.section "overw: new status menu lower 9" overwrite
  lda #$04-2
  sta menuBaseXB.b
  lda #$0E
  sta menuBaseYB.b
.ends

;===================================
; map name window
;===================================

.bank metabank_overw slot std_slot
.orga $BEFE
.section "overw: new map name window 1" SIZE $43 overwrite
  .incbin "out/script/menu/overw/str-0x2EFE.bin"
.ends

;===================================
; wider party member select menu
;===================================

.bank metabank_kernel slot std_slot
.orga $57AB
.section "kernel: wider party member select menu 1" SIZE $13 overwrite
  .incbin "out/script/menu/kernel/str-0x17AB.bin"
.ends

;===================================
; map caching
;===================================

;=====
; kernel memory
;=====

.bank metabank_kernel slot std_slot
.section "kernel: map cache 1" free
    ; src sector of cached map A
  mapCacheA:
  mapCacheA_lo:
    .db $FF
  mapCacheA_mid:
    .db $FF
  mapCacheA_hi:
    .db $FF
.ends

.bank metabank_kernel slot std_slot
.section "kernel: map cache 2" free
  mapCacheB:
  mapCacheB_lo:
    .db $FF
  mapCacheB_mid:
    .db $FF
  mapCacheB_hi:
    .db $FF
.ends

.bank metabank_kernel slot std_slot
.section "kernel: map cache 3" free
  currentMapCacheIndex:
    ; if zero, "current" map is A and "cached" (previous) map is B.
    ; if nonzero, the opposite is true.
    .db $00
.ends

;=====
; map caching routines
;=====

/*.bank metabank_overw slot std_slot
.orga $9FD4
.section "overw: map cache 4" SIZE 18 overwrite
  doTrampolineCallNew1 doMapCacheCheck
  ; return if cached map was loaded, since that means
  ; we no longer need to load a new map from disc
  lda cachedMapLoadOccurred.w
  beq +
    rts
  +:
  jmp $9FE6
.ends

.bank metabank_overw slot std_slot
.section "overw: map cache 5" free
  cachedMapLoadOccurred:
    .db $00
.ends*/

/*.bank metabank_overw slot std_slot
.orga $9FD4
.section "overw: map cache 4" SIZE 7 overwrite
  doTrampolineCallNew1 doMapCacheCheck
  rts
.ends*/

.bank metabank_overw slot std_slot
.orga $9FDB
.section "overw: map cache 4b" SIZE 37 overwrite
  ; HACK: this routine is designed to copy from mpr5 to mpr6.
  ; in order for this to work, it must be in mpr4 (i.e. the first
  ; 0x1000 bytes of overw).
  ; we have enough room to insert it at the position formerly occupied
  ; by the now-disused map->adpcm and adpcm->memory load specifiers
  ; and accompanying routines.
  
  ; 29 bytes
  ; to use, before calling:
  ; - set up mpr5 (src) with desired src page
  ; - set @dstPageCmd+1 to desired mpr6 (dst) page
  ; - set @copyCmd+1 to $A000 and @copyCmd+3 to $C000
  copyFromSlot5To6:
    tma #$40
    pha
    @dstPageCmd:
    lda #$00
    tam #$40
    ldx #$20
    -:
      @copyCmd:
      tii $A000,$C000,$0100
      inc @copyCmd+2.w
      inc @copyCmd+4.w
      dex
      bne -
    pla
    tam #$40
    
    rts
.ends

.bank metabank_new1 slot std_slot
.section "new1: map cache 5" free
  doMapCacheCheck:
;    stz cachedMapLoadOccurred.w
    
    ; it is quite possible for the target map to be the current one.
    ; for instance, this happens after every random encounter, when the game
    ; needs to reload the map the player is on.
    ; so we need to first check if the current map is the target and NOT
    ; flip the cache if so.
    jsr returnCarryClearIfCurrentMapIsTarget
    bcc +
      ; swap current map with cached
      lda currentMapCacheIndex.w
      eor #$01
      sta currentMapCacheIndex.w
    +:
    
    ;=====
    ; check if last cached map (which is now in "current map" slot)
    ; is target map
    ;=====
    
    lda currentMapCacheIndex.w
    bne @copyFromSlotB
    @copyFromSlotA:
      lda $3095.w
      cmp mapCacheA_lo.w
      bne @finishMapLoad_A
      lda $3096.w
      cmp mapCacheA_mid.w
      bne @finishMapLoad_A
      lda $3097.w
      cmp mapCacheA_hi.w
      bne @finishMapLoad_A
      
      ; matched
      rts
    @copyFromSlotB:
      lda $3095.w
      cmp mapCacheB_lo.w
      bne @finishMapLoad_B
      lda $3096.w
      cmp mapCacheB_mid.w
      bne @finishMapLoad_B
      lda $3097.w
      cmp mapCacheB_hi.w
      bne @finishMapLoad_B
      
      ; matched
      rts
    
    ;=====
    ; cache miss
    ; load new map from disc to current slot
    ;=====
    
    @finishMapLoad_A:
      ; update map num to new src
      lda $3095.w
      sta mapCacheA_lo.w
      lda $3096.w
      sta mapCacheA_mid.w
      lda $3097.w
      sta mapCacheA_hi.w
      
      lda #mapCacheAMemPage
      sta @finishMapLoad_dstOp+1.w
      bra @finishMapLoad
    @finishMapLoad_B:
      ; update map num to new src
      lda $3095.w
      sta mapCacheB_lo.w
      lda $3096.w
      sta mapCacheB_mid.w
      lda $3097.w
      sta mapCacheB_hi.w
      
      lda #mapCacheBMemPage
      sta @finishMapLoad_dstOp+1.w
    @finishMapLoad:
    -:
      ; BL = dst
      @finishMapLoad_dstOp:
      lda #$00
      sta _BL.b
      ; load src
      lda $3095.w
      sta _DL.b
      lda $3096.w
      sta _CH.b
      lda $3097.w
      sta _CL.b
      ; DH = type = mpr6
      lda #$06
      sta _DH.b
      ; AL = record count
      lda #<(mapDataSize/$800)
      sta _AL.b
      jsr CD_READ
      and #$FF
      bne -
    
    rts
  
  returnCarryClearIfCurrentMapIsTarget:
    lda currentMapCacheIndex.w
    bne @inSlotB
    @inSlotA:
      lda $3095.w
      cmp mapCacheA_lo.w
      bne @fail
      lda $3096.w
      cmp mapCacheA_mid.w
      bne @fail
      lda $3097.w
      cmp mapCacheA_hi.w
      bne @fail
      
      ; matched
      clc
      rts
    @inSlotB:
      lda $3095.w
      cmp mapCacheB_lo.w
      bne @fail
      lda $3096.w
      cmp mapCacheB_mid.w
      bne @fail
      lda $3097.w
      cmp mapCacheB_hi.w
      bne @fail
      
      ; matched
      clc
      rts
    @fail:
    sec
    rts
.ends

;===================================
; use cache when loading maps
;===================================

.macro setMawrAndPrepForWrite ARGS addr
  lda #$00
  sta $F7.b
  sta $0000.w
  st1 #<addr
  st2 #>addr
  
  lda #$02
  sta $F7.b
  sta $0000.w
.endm

.bank metabank_overw slot std_slot
.orga $9FC3
.section "overw: use map cache 1" overwrite
  doTrampolineCallNew1 loadNewMap
  jmp $9FD1
.ends

.bank metabank_new1 slot std_slot
.section "new1: use map cache 2" free
;  ; 29 bytes
;  ; to use, before calling:
;  ; - set up mpr5 (src) with desired src page
;  ; - set @dstPageCmd+1 to desired mpr6 (dst) page
;  ; - set @copyCmd+1 to $A000 and @copyCmd+3 to $C000
;  copyFromSlot5To6:
  
  loadNewMap:
    ; either load new map from memory or get it from the cache
    jsr doMapCacheCheck
    ; copy resources from new map to destinations in memory
    jmp copyCurrentMapResources
  
  copyCurrentMapResources:
    phy
    phx
      ; X = base bank for current map
      ldx currentMapCacheIndex.w
      
      tma #$20
      pha
        ; copy 1: 0x2000 bytes from map+0x0000 to page $85
        lda mapSlotToCopy1SrcTable.w,X
        tam #$20
        lda #$85
        sta copyFromSlot5To6@dstPageCmd+1.w
        lda #$A0
        sta copyFromSlot5To6@copyCmd+2.w
        lda #$C0
        sta copyFromSlot5To6@copyCmd+4.w
        phx
          jsr copyFromSlot5To6
        plx
        
        ; copy 2: 0x2000 bytes from map+0x2000 to page $84
        lda mapSlotToCopy2SrcTable.w,X
        tam #$20
        lda #$84
        sta copyFromSlot5To6@dstPageCmd+1.w
        lda #$A0
        sta copyFromSlot5To6@copyCmd+2.w
        lda #$C0
        sta copyFromSlot5To6@copyCmd+4.w
        phx
          jsr copyFromSlot5To6
        plx
        
        ; copy 3: 0x4000 bytes from map+0x8000 to vram $4B00
        
        setMawrAndPrepForWrite $4B00
        
        lda mapSlotToCopy3aSrcTable.w,X
        tam #$20
        lda #$A0
        sta @copy3aCmd+2.w
        stz @copy3aCmd+1.w
        cly
        -:
          @copy3aCmd:
          tia $A000,$0002,$0020
          
          lda @copy3aCmd+1.w
          clc
          adc #$20
          sta @copy3aCmd+1.w
          bcc +
            inc @copy3aCmd+2.w
          +:
          
          iny
          bne -
        
        lda mapSlotToCopy3bSrcTable.w,X
        tam #$20
        lda #$A0
        sta @copy3bCmd+2.w
        stz @copy3bCmd+1.w
        cly
        -:
          @copy3bCmd:
          tia $A000,$0002,$0020
          
          lda @copy3bCmd+1.w
          clc
          adc #$20
          sta @copy3bCmd+1.w
          bcc +
            inc @copy3bCmd+2.w
          +:
          
          iny
          bne -
        
        ; copy 4: 0x1000 bytes from map+0xC000 to vram $4300
        
        setMawrAndPrepForWrite $4300
        
        lda mapSlotToCopy4SrcTable.w,X
        tam #$20
        lda #$A0
        sta @copy4Cmd+2.w
        stz @copy4Cmd+1.w
        cly
        -:
          @copy4Cmd:
          tia $A000,$0002,$0020
          
          lda @copy4Cmd+1.w
          clc
          adc #$20
          sta @copy4Cmd+1.w
          bcc +
            inc @copy4Cmd+2.w
          +:
          
          iny
          cpy #$80
          bne -
      
      pla
      tam #$20
    plx
    ply
    rts
  
  mapSlotToCopy1SrcTable:
    .db mapCacheAMemPage+$0
    .db mapCacheBMemPage+$0
  
  mapSlotToCopy2SrcTable:
    .db mapCacheAMemPage+$1
    .db mapCacheBMemPage+$1
  
  mapSlotToCopy3aSrcTable:
    .db mapCacheAMemPage+$4
    .db mapCacheBMemPage+$4
  
  mapSlotToCopy3bSrcTable:
    .db mapCacheAMemPage+$5
    .db mapCacheBMemPage+$5
  
  mapSlotToCopy4SrcTable:
    .db mapCacheAMemPage+$6
    .db mapCacheBMemPage+$6
  
.ends

;===================================
; separately load sound data from
; cached maps, because that's how
; the game does it for some reason
;===================================

.bank metabank_overw slot std_slot
.orga $9CE3
.section "overw: use map cache 3" SIZE 26 overwrite
  doTrampolineCallNew1 loadMapSounds
  jmp $9CFD
.ends

.bank metabank_new1 slot std_slot
.section "new1: use map cache 4" free
;  ; 29 bytes
;  ; to use, before calling:
;  ; - set up mpr5 (src) with desired src page
;  ; - set @dstPageCmd+1 to desired mpr6 (dst) page
;  ; - set @copyCmd+1 to $A000 and @copyCmd+3 to $C000
;  copyFromSlot5To6:
  
  loadMapSounds:
    phx
      ; X = base bank for current map
      ldx currentMapCacheIndex.w
      
      tma #$20
      pha
        ; copy 1: 0x2000 bytes from map+0x4000 to page $87
        lda mapSoundSlotToCopySrcTable.w,X
        tam #$20
        lda #$87
        sta copyFromSlot5To6@dstPageCmd+1.w
        lda #$A0
        sta copyFromSlot5To6@copyCmd+2.w
        lda #$C0
        sta copyFromSlot5To6@copyCmd+4.w
        phx
          jsr copyFromSlot5To6
        plx
      pla
      tam #$20
    plx
    rts
  
  ; TODO: what is page 3 of the map data used for, if anything?
  mapSoundSlotToCopySrcTable:
    .db mapCacheAMemPage+$2
    .db mapCacheBMemPage+$2
.ends

;===================================
; battle caching
;===================================

/*.bank metabank_kernel slot std_slot
.orga $7E05
.section "kernel: use battle cache 1" SIZE 8 overwrite
  doTrampolineCallNew1 doNewLoadBattleModule
  rts
.ends

.bank metabank_new1 slot std_slot
.section "new1: use battle cache 2" free
  doNewLoadBattleModule:
    ; dstaddr = 0x0000
    stz _CL.b
    stz _CH.b
    ; via = mpr6
    lda #$06
    sta _DH.b
    ; src bank
    lda #battleCacheMemPage
    sta _BL.b
    ; size
    lda #<(battleCacheSectorCount*$800)
    sta _AL.b
    lda #>(battleCacheSectorCount*$800)
    sta _AH.b
    jsr AD_WRITE
    rts
.ends*/

.bank metabank_kernel slot std_slot
.orga $7E05
.section "kernel: use battle cache 1" overwrite
  ; do not load battle module from cd to adpcm,
  ; since we already have it cached
  rts
.ends

; TODO: removing this routine frees up a decent chunk of space
; that would be nice to unbackground, but since it's currently
; overwritten at some times by the kernel copying routine,
; it can't be used. should probably find somewhere else to put
; the copy routines temporarily and then unbackground this.
.bank metabank_kernel slot std_slot
.orga $7E17
.section "kernel: use battle cache 3" SIZE $10 overwrite
  doTrampolineCallNew1 distributeCachedBattleModuleResources
  ; ???
  jsr $AB6E
  doTrampolineCallNew1 distributeCachedBattleModuleResources2
  rts
.ends

.bank metabank_new1 slot std_slot
.section "new1: use battle cache 4" free
  distributeCachedBattleModuleResources:
    lda $2B.b
    pha
    lda $2C.b
    pha
    lda $7B.b
    pha
    lda $7C.b
    pha
    phx
    phy
    
      tma #$20
      pha
        
        ;=====
        ; main code block
        ;=====
        
        ; split across three transfers to deal with banking
        
        ; src page
        lda #(battleCacheMemPage+($0000/$2000))
        tam #$20
        ; src
        lda #<$A000
        sta $2B.b
        lda #>$A000
        sta $2C.b
        ; dst
        lda #<$D000
        sta $7B.b
        lda #>$D000
        sta $7C.b
        ; dst page
        ldy #$82
        ; size = 0x20 loops of 0x80 = 0x1000 bytes
;        ldx #$20
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$10
        jsr bigCopyViaKernelAndMpr45
        
        ; src page
        lda #(battleCacheMemPage+($1000/$2000))
        tam #$20
        ; src
        lda #<$B000
        sta $2B.b
        lda #>$B000
        sta $2C.b
        ; dst
        lda #<$C000
        sta $7B.b
        lda #>$C000
        sta $7C.b
        ; dst page
        ldy #$83
        ; size = 0x20 loops of 0x80 = 0x1000 bytes
;        ldx #$20
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$10
        jsr bigCopyViaKernelAndMpr45
        
        ; src page
        lda #(battleCacheMemPage+($2000/$2000))
        tam #$20
        ; src
        lda #<$A000
        sta $2B.b
        lda #>$A000
        sta $2C.b
        ; dst
        lda #<$D000
        sta $7B.b
        lda #>$D000
        sta $7C.b
        ; dst page
        ldy #$83
        ; size = 0x20 loops of 0x80 = 0x1000 bytes
;        ldx #$20
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$10
        jsr bigCopyViaKernelAndMpr45
        
        ;=====
        ; battle_text2
        ;=====
        
        ; src page
        lda #(battleCacheMemPage+($A000/$2000))
        tam #$20
        ; src
        lda #<$A000
        sta $2B.b
        lda #>$A000
        sta $2C.b
        ; dst
        lda #<$C000
        sta $7B.b
        lda #>$C000
        sta $7C.b
        ; dst page
        ldy #$85
        ; size = 0x30 loops of 0x80 = 0x1800 bytes
;        ldx #$30
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$18
        jsr bigCopyViaKernelAndMpr45
        
        ;=====
        ; unknown1
        ;=====
        
        ; src page
        lda #(battleCacheMemPage+($3000/$2000))
        tam #$20
        ; src
        lda #<$B000
        sta $2B.b
        lda #>$B000
        sta $2C.b
        ; dst
        lda #<$D800
        sta $7B.b
        lda #>$D800
        sta $7C.b
        ; dst page
        ldy #$85
        ; size = 0x10 loops of 0x80 = 0x800 bytes
;        ldx #$10
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$08
        jsr bigCopyViaKernelAndMpr45
      
      pla
      tam #$20
    
    ply
    plx
    pla
    sta $7C.b
    pla
    sta $7B.b
    pla
    sta $2C.b
    pla
    sta $2B.b
    rts
    
  distributeCachedBattleModuleResources2:
    lda $2B.b
    pha
    lda $2C.b
    pha
    lda $7B.b
    pha
    lda $7C.b
    pha
    phx
    phy
    
      tma #$20
      pha
        
        ;=====
        ; battle_text
        ;=====
        
        ; src page
        lda #(battleCacheMemPage+($8000/$2000))
        tam #$20
        ; src
        lda #<$A000
        sta $2B.b
        lda #>$A000
        sta $2C.b
        ; dst
        lda #<$C000
        sta $7B.b
        lda #>$C000
        sta $7C.b
        ; dst page
        ldy #$84
        ; size = 0x40 loops of 0x80 = 0x2000 bytes
;        ldx #$40
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$20
        jsr bigCopyViaKernelAndMpr45
      
      pla
      tam #$20
    
    ply
    plx
    pla
    sta $7C.b
    pla
    sta $7B.b
    pla
    sta $2C.b
    pla
    sta $2B.b
    rts
.ends

.bank metabank_battle slot std_slot
.orga $BB22
.section "battle: use battle cache 5" overwrite
  doTrampolineCallNew1 loadBattleEnemyDataFromCache
  nop
.ends

.bank metabank_battle slot std_slot
.orga $BB4C
.section "battle: use battle cache 6" overwrite
  doTrampolineCallNew1 loadBattleEnemyDataFromCache
  nop
.ends

.bank metabank_new1 slot std_slot
.section "new1: use battle cache 7" free
  loadBattleEnemyDataFromCache:
    ; make up work
    sta $F8.b
    stz $F9.b
    
    lda $2B.b
    pha
    lda $2C.b
    pha
    lda $7B.b
    pha
    lda $7C.b
    pha
    phx
    phy
    
      tma #$20
      pha
        
        ; we enter this with params set up for an AD_READ call:
        ; - AX = length in bytes (always 0x30)
        ; - BX = dst ptr, which is a location in main memory ($2000-$4000)
        ; - CX = src addr in adpcm buffer
        ; - DH = type (00 = local)
        
        ; src page
        lda _CH.b
        lsr
        lsr
        lsr
        lsr
        lsr
        clc
        adc #battleCacheMemPage
        tam #$20
        ; AS EXPLAINED BELOW, the enemy data straddles banks.
        ; but luckily we're copying to the main RAM, so we can just load
        ; up the next bank of enemy data in the dst slot and have everything
        ; work out nicely.
        ; no need to agonizingly split the transfer up into two parts,
        ; why would anyone ever think that? ahaha
        ina
        tay
        ; src ptr
        ; add 0x6000 to get mpr5 pointer.
        ; if high byte of result >= 0xC0, subtract 0x2000.
        lda _CL.b
        sta $2B.b
        lda _CH.b
        clc
        adc #$60
        cmp #$C0
        bcc +
          ; carry is set if we're running this code,
          ; so no need to sec here
          sbc #$20
        +:
        sta $2C.b
        ; dst
        lda _BL.b
        sta $7B.b
        lda _BH.b
        sta $7C.b
        ; dst page (dummy, we aren't writing to this area)
;        ldy #$80
        ; size = 0x1 loop of 0x30 = 0x30 bytes
        ldx #$1
;        lda #$30
        ; HACK: we assume size is < 0x100 bytes
        lda _AL.b
        jsr copyViaKernelAndMpr45

/*        ; oops, the enemy data is 0x3000 bytes long and straddles pages!
        ; so now the game crashes whenever you encounter the "zombie master"
        ; enemy (which is found only in the very last goddamn dungeon,
        ; so guess how much fun it was discovering this at the very end of
        ; a long tedious editing session!)
        
        ; laboriously split the transfer into two pieces just in case
        ; we're at the edge of the bank
        
        ; add size of transfer to base address and check if this results
        ; in an address >= 0xC000, i.e. we can't copy it in one go
        ; (will also misdetect the case where it aligns directly to the
        ; end of the bank, but i'm not inclined to care at this point)
        lda $2B.b
        clc
        adc _AL.b
        sta @overflowCmd+1.w
        cla
        adc $2C.b
        cmp #$C0
        bcs @wontFit
        @willFit:
          ; one copy
          lda _AL.b
          jsr copyViaKernelAndMpr45
          bra @done
        @wontFit:
          ; get size of first transfer (0x30 - overflow size)
          lda _AL.b
          sec
          sbc @overflowCmd+1.w
          ; make sure to catch the case where the end of the target area
          ; aligns directly to the end of the bank
          beq @willFit
;          sta @firstSizeCmd+1.w
          jsr copyViaKernelAndMpr45
          
          ; we know the next data to copy comes from the next bank and
          ; starts at $C000
          stz $2B.b
          lda #$C0
          sta $2C.b
          
          ; add first transfer size to dst
          ; (i feel compelled to note, even though i'm no longer using this code,
          ; that this is already done by the copy routine)
/*          @firstSizeCmd:
          lda #$00
          clc
          adc $7B.b
          sta $7B.b
          cla
          adc $7C.b
          sta $7C.b*/
          
          ; dummy dstbank, 
          ldy #$80
          ldx #$1
          
          ; WAIT, WHY AM I WRITING THIS GIANT ELABORATE BLOCK OF CODE
          ; WE'RE COPYING TO MAIN MEMORY SO ALL I HAD TO DO WAS PUT
          ; THE NEXT BANK NUMBER IN Y
          
          ; this is the number of bytes by which we overflowed the bank,
          ; i.e. the size of the second transfer
          @overflowCmd:
          lda #$00
        @done:*/
      
      pla
      tam #$20
    
    ply
    plx
    pla
    sta $7C.b
    pla
    sta $7B.b
    pla
    sta $2C.b
    pla
    sta $2B.b
    rts
.ends

;===================================
; wacky scheme to temporarily upload
; copy routines into kernel in order
; to work around the fact that there's
; not enough space left to put them
; there normally
;===================================

; this block of code (which relates to loading battle resources)
; is temporarily overwritten with the copy routines
.define kernelCopied_copyRoutine_loadAddr $7D96

.define kernelCopiedRoutine_maxSize $40

.bank metabank_new1 slot std_slot
.section "new1: kernel-based copy commands 1" free
  ; do a local copy from $2B-2C to $7B-7C,
  ; where:
  ; - Y = bank to page in to MPR6 during copy
  ; - A = number of bytes to copy per loop
  ; - X = number of copy loops to run
  copyViaKernelAndMpr45:
    ; set up routine
    sta kernelCopied_copyRoutine@innerLoopCmpCmd+1.w
    stx kernelCopied_copyRoutine@outerLoopCmpCmd+1.w
    
    ; copy code to be overwritten to buffer
    tii kernelCopied_copyRoutine_loadAddr,kernelCopiedRoutine_oldCodeBuffer,(kernelCopied_copyRoutine_end-kernelCopied_copyRoutine)
    
    ; copy routine to kernel
    tii kernelCopied_copyRoutine,kernelCopied_copyRoutine_loadAddr,(kernelCopied_copyRoutine_end-kernelCopied_copyRoutine)
    
    ; set up routine
    jsr kernelCopied_copyRoutine_loadAddr
    
    ; restore original code
    ; copy saved routine to kernel
    tii kernelCopiedRoutine_oldCodeBuffer,kernelCopied_copyRoutine_loadAddr,(kernelCopied_copyRoutine_end-kernelCopied_copyRoutine)
    rts
  
  ; do a local copy from $2B-2C to $7B-7C,
  ; where:
  ; - Y = bank to page in to MPR6 during copy
  ; - X = number of 0x100-byte copy loops to run
  bigCopyViaKernelAndMpr45:
    ; set up routine
;    stx kernelCopied_bigCopyRoutine@outerLoopCmpCmd+1.w
    
    ; copy code to be overwritten to buffer
    tii kernelCopied_copyRoutine_loadAddr,kernelCopiedRoutine_oldCodeBuffer,(kernelCopied_bigCopyRoutine_end-kernelCopied_bigCopyRoutine)
    
    ; copy routine to kernel
    tii kernelCopied_bigCopyRoutine,kernelCopied_copyRoutine_loadAddr,(kernelCopied_bigCopyRoutine_end-kernelCopied_bigCopyRoutine)
    
    ; set up routine
    jsr kernelCopied_copyRoutine_loadAddr
    
    ; restore original code
    ; copy saved routine to kernel
    tii kernelCopiedRoutine_oldCodeBuffer,kernelCopied_copyRoutine_loadAddr,(kernelCopied_bigCopyRoutine_end-kernelCopied_bigCopyRoutine)
    rts
  
/*  loadIn_kernelCopied_copyRoutine:
    ; copy code to be overwritten to buffer
    tii kernelCopied_copyRoutine_loadAddr,kernelCopiedRoutine_oldCodeBuffer,(kernelCopied_copyRoutine_end-kernelCopied_copyRoutine)
    ; copy routine to kernel
    tii kernelCopied_copyRoutine,kernelCopied_copyRoutine_loadAddr,(kernelCopied_copyRoutine_end-kernelCopied_copyRoutine)
    rts
  
  loadOut_kernelCopied_copyRoutine:
    ; copy saved routine to kernel
    tii kernelCopiedRoutine_oldCodeBuffer,kernelCopied_copyRoutine_loadAddr,(kernelCopied_copyRoutine_end-kernelCopied_copyRoutine)
    rts*/
  
  ; *** MUST BE RELOCATABLE!! ***
  ; (or i could just compute target offsets manually from the predetermined
  ; load address, but eh)
  ; do not use self-modifying code within this routine,
  ; or jmp for branches within the routine.
  ; max size of routine is kernelCopiedRoutine_maxSize.
  ;
  ; to use, set the following:
  ;   Y = bank to page in to MPR6
  ;   $2B-2C = src pointer
  ;   $7B-7C = dst ptr
  ;   @innerLoopCmpCmd+1 = bytes to copy per loop (must not be zero)
  ;   @outerLoopCmpCmd+1 = number of copy loops
  ; 
  ; do not exceed bank boundaries
  kernelCopied_copyRoutine:
    tma #$40
    pha
    tya
    tam #$40
      clx
      @copyLoopOuter:
        cly
        @copyLoopInner:
          lda ($2B.b),Y
          sta ($7B.b),Y
          
          iny
          @innerLoopCmpCmd:
          cpy #$00
          bne @copyLoopInner
        
        tya
        clc
        adc $2B.b
        sta $2B.b
        bcc +
          inc $2C.b
        +:
        
        tya
        clc
        adc $7B.b
        sta $7B.b
        bcc +
          inc $7C.b
        +:
        
        inx
        @outerLoopCmpCmd:
        cpx #$00
        bne @copyLoopOuter
    pla
    tam #$40
    
    rts
  kernelCopied_copyRoutine_end:
  
  kernelCopied_bigCopyRoutine:
    tma #$40
    pha
    tya
    tam #$40
      ; set up src
      lda $2B.b
      sta kernelCopied_copyRoutine_loadAddr+((@transferCmd+1)-kernelCopied_bigCopyRoutine).w
      lda $2C.b
      sta kernelCopied_copyRoutine_loadAddr+((@transferCmd+2)-kernelCopied_bigCopyRoutine).w
      ; set up dst
      lda $7B.b
      sta kernelCopied_copyRoutine_loadAddr+((@transferCmd+3)-kernelCopied_bigCopyRoutine).w
      lda $7C.b
      sta kernelCopied_copyRoutine_loadAddr+((@transferCmd+4)-kernelCopied_bigCopyRoutine).w
      
      @copyLoopOuter:
        @transferCmd:
        tii $0000,$0000,$0100
        
        ; advance src/dst by 0x100 bytes each
        inc kernelCopied_copyRoutine_loadAddr+((@transferCmd+2)-kernelCopied_bigCopyRoutine).w
        inc kernelCopied_copyRoutine_loadAddr+((@transferCmd+4)-kernelCopied_bigCopyRoutine).w
        
        @outerLoopCmpCmd:
        dex
        bne @copyLoopOuter
        
    pla
    tam #$40
    rts
  kernelCopied_bigCopyRoutine_end:
  
  ; storage for routines in kernel that are temporarily copied over
  kernelCopiedRoutine_oldCodeBuffer:
    .ds kernelCopiedRoutine_maxSize,$00
.ends

;===================================
; overw caching
;===================================

;.bank metabank_kernel slot std_slot
;.orga $7E05
;.section "kernel: overw cache 1" SIZE 8 overwrite
;  
;.ends

.bank metabank_new1 slot std_slot
.section "new1: overw cache 1" free
  overwCache9800:
    .if PRELINKED == 0
      .ds $800,$00
    .else
      .incbin "out/base/overw_13.bin" SKIP $800 READ $800
    .endif
  
  loadOverwCache9800:
    phx
      lda #<overwCache9800
      sta @copyCmd+1.w
      lda #>overwCache9800
      sta @copyCmd+2.w
      lda #<$9800
      sta @copyCmd+3.w
      lda #>$9800
      sta @copyCmd+4.w
      ldx #8
      -:
        @copyCmd:
        tii $0000,$9800,$0100
        inc @copyCmd+2.w
        inc @copyCmd+4.w
        dex
        bne -
    plx
    rts
  
  loadOverwCache:
    ; load $9000-$A000 segment of overw
    jsr loadOverwCache9800
    doTrampolineCallNew2 loadOverwCache9000
    
    ; now that the $9000-$A000 range of overw is loaded,
    ; we can make use of its copyFromSlot5To6 routine to
    ; load the remaining data to $A000-$C000
    tma #$20
    pha
    lda #overwCacheMemPage
    tam #$20
      lda #$83
      sta copyFromSlot5To6@dstPageCmd+1.w
      lda #$A0
      sta copyFromSlot5To6@copyCmd+2.w
      lda #$C0
      sta copyFromSlot5To6@copyCmd+4.w
      phx
        jsr copyFromSlot5To6
      plx
    pla
    tam #$20
    
    rts
.ends

.bank metabank_new2 slot std_slot
.section "new2: overw cache 2" free
  overwCache9000:
    .if PRELINKED == 0
      .ds $800,$00
    .else
      .incbin "out/base/overw_13.bin" SKIP $0 READ $800
    .endif
  
  loadOverwCache9000:
    phx
      lda #<overwCache9000
      sta @copyCmd+1.w
      lda #>overwCache9000
      sta @copyCmd+2.w
      lda #<$9000
      sta @copyCmd+3.w
      lda #>$9000
      sta @copyCmd+4.w
      ldx #8
      -:
        @copyCmd:
        tii $0000,$9000,$0100
        inc @copyCmd+2.w
        inc @copyCmd+4.w
        dex
        bne -
    plx
    rts
.ends

.bank metabank_kernel slot std_slot
.orga $510A
.section "kernel: use overw cache 1" overwrite
  doTrampolineCallNew1 loadOverwCache
  jmp $5115
.ends

;===================================
; info caching
;===================================

.bank metabank_overw slot std_slot
.orga $BF53
.section "overw: use info cache 1" overwrite
  doTrampolineCallNew1 loadInfoCache
  jmp $BF5E
.ends

.bank metabank_new1 slot std_slot
.section "new1: info cache 2" free
  loadInfoCache:
    lda $2B.b
    pha
    lda $2C.b
    pha
    lda $7B.b
    pha
    lda $7C.b
    pha
    phx
    phy
    
      tma #$20
      pha
        
        ;=====
        ; 
        ;=====
        
        ; src page
        ; (a copy of info has been placed in the last 3 sectors of
        ; unknown1, which are otherwise unused)
        lda #(battleCacheMemPage+($3800/$2000))
        tam #$20
        ; src
        lda #<$B800
        sta $2B.b
        lda #>$B800
        sta $2C.b
        ; dst
        lda #<$C000
        sta $7B.b
        lda #>$C000
        sta $7C.b
        ; dst page
        ldy #$85
        ; size = 0x1800 bytes
;        ldx #$40
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$8
        jsr bigCopyViaKernelAndMpr45
        
        ; src page
        ; (a copy of info has been placed in the last 3 sectors of
        ; unknown1, which are otherwise unused)
        lda #(battleCacheMemPage+($4000/$2000))
        tam #$20
        ; src
        lda #<$A000
        sta $2B.b
        lda #>$A000
        sta $2C.b
        ; dst
        lda #<$C800
        sta $7B.b
        lda #>$C800
        sta $7C.b
        ; dst page
        ldy #$85
        ; size = 0x1800 bytes
;        ldx #$40
;        lda #$80
;        jsr copyViaKernelAndMpr45
        ldx #$10
        jsr bigCopyViaKernelAndMpr45
      
      pla
      tam #$20
    
    ply
    plx
    pla
    sta $7C.b
    pla
    sta $7B.b
    pla
    sta $2C.b
    pla
    sta $2B.b
    rts
.ends

;==============================================================================
; shop
;==============================================================================

;===================================
; double override text speed
;===================================

.bank metabank_shop slot std_slot
.orga $CCCA
.section "shop: double override text speed" overwrite
;  lda #$01
  lda #$00
.ends

;===================================
; new buy/sell menu
;===================================

.bank metabank_shop slot std_slot
.orga $C9CD
.section "shop: new buy/sell menu 1" SIZE $29 overwrite
  .incbin "out/script/menu/shop/str-0x9CD.bin"
.ends

.bank metabank_shop slot std_slot
.orga $C9B2
.section "shop: new buy/sell menu 2" overwrite
  ; 16-bit access
  lda #$00
  sta menuBaseX.w
  lda #$02
  sta menuBaseY.w
.ends

;===================================
; new storage menu
;===================================

.bank metabank_shop slot std_slot
.orga $D925
.section "shop: new storage menu 1" SIZE $B8 overwrite
  .incbin "out/script/menu/shop/str-0x1925.bin"
.ends

.bank metabank_shop slot std_slot
.orga $D906
.section "shop: new storage menu 2" overwrite
  lda #$00
  sta menuBaseXB.b
  lda #$02
  sta menuBaseYB.b
.ends

;===================================
; new player money window
;===================================

/*.bank metabank_shop slot std_slot
.orga $CD06
.section "shop: new player money window 1" SIZE $16 overwrite
  .incbin "out/script/menu/shop/str-0xD06.bin"
.ends

.bank metabank_shop slot std_slot
.orga $CCF8
.section "shop: new player money window 2" overwrite
  ; 16-bit access
  lda #$14
  sta menuBaseX.w
  lda #$08
  sta menuBaseY.w
.ends*/

; moving to kernel so this can be larger
.bank metabank_kernel slot std_slot
.section "shop: new player money window 1" free
  newPlayerMoneyWindow:
    .incbin "out/script/menu/shop/str-0xD06.bin"
.ends

.bank metabank_shop slot std_slot
.orga $CCF0
.section "shop: new player money window 2" overwrite
  lda #<newPlayerMoneyWindow
  sta $2B.b
  lda #>newPlayerMoneyWindow
  sta $2C.b
  ; 16-bit access
  lda #$14
  sta menuBaseX.w
  lda #$08
  sta menuBaseY.w
.ends

;===================================
; new storage money window
;===================================

.bank metabank_shop slot std_slot
.orga $DD7A
.section "shop: new storage money window 1" SIZE $1B overwrite
  .incbin "out/script/menu/shop/str-0x1D7A.bin"
.ends

.bank metabank_shop slot std_slot
.orga $DD6E
.section "shop: new storage money window 2" overwrite
  lda #$00+$2
  sta menuBaseXB.b
  lda #$08
  sta menuBaseYB.b
.ends

;===================================
; new buyable item template
;===================================

.bank metabank_shop slot std_slot
.orga $CC53
.section "shop: new buyable item template 1" SIZE $2D overwrite
  .incbin "out/script/menu/shop/str-0xC53.bin"
.ends

;===================================
; use narrowing for storage menus
;===================================

.bank metabank_shop slot std_slot
.orga $DC3D
.section "shop: use narrowing for storage 1" overwrite
  jsr printStringRawWithItemNarrowing
.ends

;===================================
; gamble: show new player bet sprite
;===================================

.bank metabank_shop slot std_slot
.orga $D242
.section "shop: gamble player bet sprite 1" overwrite
  doTrampolineCallNew1 setUpGamblePlayerBetSprite
  jmp $D250
.ends

.bank metabank_new1 slot std_slot
.section "shop: gamble player bet sprite 2" free
  setUpGamblePlayerBetSprite:
    ; make up work
    sta $221D.w
    
    ; sprite size = 32px
    lda #$01
    sta $221E.w
    
    ; finish up: A = low byte of pattern num
    lda $3411.w
    and #$01
    asl
    clc
;    adc #$0E
    adc #$10
    
    rts
.ends

;===================================
; gamble: show new player bet sprite
;===================================

.bank metabank_shop slot std_slot
.orga $D0D6
.section "shop: gamble result sprite 1" overwrite
  doTrampolineCallNew1 setUpGambleResultSprite
  jmp $D0E2
.ends

.bank metabank_new1 slot std_slot
.section "shop: gamble result sprite 2" free
  setUpGambleResultSprite:
    ; sprite size = 32px
    lda #$01
    sta $221E.w
    
    ; sum dice results
    lda $309F.w
    clc
    adc $30A0.w
    ; get parity
    and #$01
    asl
    clc
    ; add base spritenum
;    adc #$0E
    adc #$10
    
    rts
.ends

;==============================================================================
; save/load
;==============================================================================

.unbackground metabank_saveload*sizeOfMetabank+$B3A0 metabank_saveload*sizeOfMetabank+$BFFF

;===================================
; new continue/copy/erase menu
;===================================

.bank metabank_saveload slot std_slot
.orga $9C2C
.section "saveload: new continue/copy/erase menu 1" SIZE $2B overwrite
  .incbin "out/script/menu/saveload/str-0xC2C.bin"
.ends

.bank metabank_saveload slot std_slot
.orga $9BF6
.section "saveload: new continue/copy/erase menu 2" overwrite
  ; mysteriously written using absolute addressing despite being
  ; zero-page addresses
  lda #$00
  sta menuBaseX.w
  lda #$04
  sta menuBaseY.w
.ends

.bank metabank_saveload slot std_slot
.orga $9C19
.section "saveload: new continue/copy/erase menu 3" overwrite
  ; mysteriously written using absolute addressing despite being
  ; zero-page addresses
  lda #$00
  sta menuBaseX.w
  lda #$04
  sta menuBaseY.w
.ends

;===================================
; new file menu 1
;===================================

.bank metabank_saveload slot std_slot
.orga $9FCE
.section "saveload: new file menu 1" SIZE $19D overwrite
  .incbin "out/script/menu/saveload/str-0xFCE.bin"
.ends

.bank metabank_saveload slot std_slot
.orga $A16B
.section "saveload: new file menu 2" SIZE $19D overwrite
  .incbin "out/script/menu/saveload/str-0x116B.bin"
.ends

.bank metabank_saveload slot std_slot
.orga $9FB8
.section "saveload: new file menu 3" overwrite
  ; mysteriously written using absolute addressing despite being
  ; zero-page addresses
  lda #$07
  sta menuBaseX.w
  lda #$02
  sta menuBaseY.w
.ends

;===================================
; title demo subtitle init
;===================================

.bank metabank_saveload slot std_slot
.orga $AF11
.section "saveload: demo subtitle init 1" overwrite
  jsr doDemoSubtitleInit
  nop
.ends

.bank metabank_saveload slot std_slot
.section "saveload: demo subtitle init 2" free
  doDemoSubtitleInit:
    ; first, shut off the subtitles to ensure they aren't accessed
    ; during init
    stz subtitleEngineOn.w
    
    doTrampolineCallNew3 doDemoSubtitleInit_sub
    
    ; turn on subtitles
    inc subtitleEngineOn.w
    
    ; make up work
    lda #$EF
    sta $C7.b
    rts
.ends

.bank metabank_new3 slot std_slot
.section "new3: demo subtitle init 3" free
  doDemoSubtitleInit_sub:
    lda #<introDemoSubtitles
    sta subtitleScriptPtr+0.w
    lda #>introDemoSubtitles
    sta subtitleScriptPtr+1.w
    
    jmp resetForScriptStart
  
  
/*  doMasakadoSubtitleInit_sub:
    lda #<masakadoSubtitles
    sta subtitleScriptPtr+0.w
    lda #>masakadoSubtitles
    sta subtitleScriptPtr+1.w
    
    jmp resetForScriptStart*/
.ends

;===================================
; stop subtitles after title
; sequence ends (e.g. due to
; pressing run on the title screen)
;===================================

.bank metabank_saveload slot std_slot
.orga $9015
.section "saveload: demo subtitle stop 1" overwrite
  jsr doDemoSubtitleStop
.ends

.bank metabank_saveload slot std_slot
.section "saveload: demo subtitle stop 2" free
  doDemoSubtitleStop:
    stz subtitleEngineOn.w
    
    ; make up work
    jmp $40E2
.ends

;===================================
; do not set up backup track using
; CD_BASE at boot
;===================================

; the backup track is untranslated, and we don't want it to be used in
; the event of a read error.
; we have already set up the track bases ourselves at this point,
; so we need to remove this call to CD_BASE during boot so the game doesn't
; override our settings.

.bank metabank_saveload slot std_slot
.orga $903E
.section "saveload: no cd_base 1" overwrite
  nop
  nop
  nop
.ends

;==============================================================================
; info
;==============================================================================

;===================================
; new info type menu
;===================================

.bank metabank_info slot std_slot
.orga $C06B
.section "info: new info type menu 1" SIZE $47 overwrite
  .incbin "out/script/menu/info/str-0x6B.bin"
.ends

.bank metabank_info slot std_slot
.orga $C050
.section "info: new info type menu 2" overwrite
  ; mysteriously written using absolute addressing despite being
  ; zero-page addresses
  lda #$14-$4
  sta menuBaseX.w
  lda #$08
  sta menuBaseY.w
.ends

;==============================================================================
; battle_text
;==============================================================================

;===================================
; new attack menus
;===================================

.bank metabank_battle_text slot std_slot
.orga $C131
.section "battle_text: new attack menu 1" SIZE $5B overwrite
  .incbin "out/script/menu/battle/str-0x131.bin"
.ends

.bank metabank_battle_text slot std_slot
.orga $C227
.section "battle_text: new attack menu 2" SIZE $59 overwrite
  .incbin "out/script/menu/battle/str-0x227.bin"
.ends

.bank metabank_battle_text slot std_slot
.orga $C294
.section "battle_text: new attack menu 3" SIZE $59 overwrite
  .incbin "out/script/menu/battle/str-0x294.bin"
.ends

.bank metabank_battle_text slot std_slot
.orga $C052
.section "battle_text: new attack menu 4" overwrite
  ; mysteriously written using absolute addressing despite being
  ; zero-page addresses
  lda #$12-$2
  sta menuBaseX.w
  lda #$14
  sta menuBaseY.w
.ends

;==============================================================================
; battle
;==============================================================================

;===================================
; new magic name buffer
;===================================

; reuse old enemy name buffer (0x30 bytes) as new magic name buffer
.define newMagicNameBuffer $386D

.bank metabank_battle slot std_slot
.orga $9433
.section "battle: new magic name buffer 1" overwrite
  ; original read loop assumes 16-bit sjis input;
  ; we only need 8-bit
  @loop:
    lda ($2B.b),Y
    sta newMagicNameBuffer.w,Y
    iny
    cmp #$FF
    bne @loop
  jmp $9445
.ends

.bank metabank_battle slot std_slot
.orga $9445
.section "battle: new magic name buffer 2" overwrite
  ; use new buffer address
  lda #<newMagicNameBuffer
  sta $33FB.w
  lda #>newMagicNameBuffer
  sta $33FC.w
.ends

/*.bank metabank_kernel slot std_slot
.section "battle: new magic name buffer 3" free
  ; original buffer appears to be too small (11 bytes?),
  ; so we make a new one in kernel memory
  newMagicNameBuffer:
    .ds 24,$00
.ends*/

;===================================
; new enemy name buffer
;===================================

.bank metabank_kernel slot std_slot
.section "battle: new enemy name buffer 1" free
  ; original buffer appears to be too small (0x30 bytes?),
  ; so we make a new one in kernel memory
  newEnemyNameBuffer:
    .ds $50,$00
.ends

.bank metabank_battle slot std_slot
.orga $BA80
.section "battle: new enemy name buffer 2" overwrite
  lda #<newEnemyNameBuffer
  sta $B4.b
  lda #>newEnemyNameBuffer
  sta $B5.b
.ends

;===================================
; follow redirects when copying
; battletext2 messages
;===================================

/*.bank metabank_battle slot std_slot
.orga $9271
.section "battle: follow battletext2 redirects 1" overwrite
  jsr checkForBattleText2Redirect
.ends

.bank metabank_kernel slot std_slot
.section "battle: follow battletext2 redirects 2" free
  checkForBattleText2Redirect:
    ; make up work
    sta ($11.b)
    iny
    
    ; check first byte of message for local redirect
    lda ($2B.b),Y
    cmp #opcode_localRedirect
    bne +
      ; redirect src to target of redirect
      iny
      lda ($2B.b),Y
      pha
        iny
        lda ($2B.b),Y
        sta $2C.b
      pla
      sta $2B.b
      
      ; urrrrrrrgh
      lda $2B.b
      bne ++
        dec $2C.b
      ++:
      dec $2B.b
      
      ldy #1
    +:
    
    rts
.ends*/

.bank metabank_battle slot std_slot
.orga $ABBC
.section "battle: follow battletext2 redirects 1" overwrite
  jsr checkForBattleText2Redirect
.ends

.bank metabank_kernel slot std_slot
.section "battle: follow battletext2 redirects 2" free
  checkForBattleText2Redirect:
    ; make up work
    sta $16.b
    
    ; check first byte of message (after presumed 1b type) for local redirect
    ldy #1
    lda ($15.b),Y
    cmp #opcode_localRedirect
    bne +
      ; "copy" type byte
      ; (it's always 3, so no need to bother)
;      cly
;      lda ($15.b),Y
;      sta $35C6.w,Y
;      iny
;      iny
      lda #3
      sta $35C6.w
      
      ; redirect src to target of redirect
      iny
      lda ($15.b),Y
      pha
        iny
        lda ($15.b),Y
        sta $16.b
      pla
      sta $15.b
      
      ; decrement src so that Y will be in sync with position
      ; of both src and dst buffers
      lda $15.b
      bne ++
        dec $16.b
      ++:
      dec $15.b
      
      ldy #1
      rts
    +:
    
    ; make up work
    cly
    rts
.ends

;===================================
; follow redirects when copying
; generic_text item use messages
;===================================

.bank metabank_battle slot std_slot
.orga $926C
.section "battle: follow generic_text item use redirects 1" overwrite
  jsr checkForGenericTextItemUseRedirect
  ; don't copy type byte
  cly
  nop
  nop
  nop
.ends

.bank metabank_kernel slot std_slot
.section "battle: follow generic_text item use redirects 2" free
  checkForGenericTextItemUseRedirect:
    ; make up work
    sta $2C.b
    
    ; "copy" type byte
    ; (it's always 3, so no need to bother)
    lda #3
    sta $35C6.w
    
    ; check first byte of message (after presumed 1b type) for local redirect
    ldy #1
    lda ($2B.b),Y
    cmp #opcode_localRedirect
    bne +
      ; redirect src to target of redirect
      iny
      lda ($2B.b),Y
      pha
        iny
        lda ($2B.b),Y
        sta $2C.b
      pla
      sta $2B.b
      
      ; decrement src so that Y will be in sync with position
      ; of both src and dst buffers
      lda $2B.b
      bne ++
        dec $2C.b
      ++:
      dec $2B.b
      
;      cly
;      rts
    +:
    
    rts
.ends

/*.bank metabank_battle slot std_slot
.orga $926C
.section "battle: follow generic_text item use redirects 1" overwrite
  jsr checkForGenericTextItemUseRedirect
  ; don't copy type byte
  cly
  nop
  nop
  nop
.ends

; unbackground unused placeholder message...
.unbackground metabank_generic_text*sizeOfMetabank+$C000+$064F metabank_generic_text*sizeOfMetabank+$C000+$06BC

; TODO: is it really okay to put this in the generic_text block?
; i don't see how this routine could ever possibly be trying to
; copy from anywhere else, so it SHOULDN'T need to go in the kernel...
; but then why is it copying anything in the first place? ugh.
.bank metabank_generic_text slot std_slot
.section "battle: follow generic_text item use redirects 3" free
  checkForGenericTextItemUseRedirect:
    ; make up work
    sta $2C.b
    
    ; "copy" type byte
    ; (it's always 3, so no need to bother)
    lda #3
    sta $35C6.w
    
    ; check first byte of message (after presumed 1b type) for local redirect
    ldy #1
    lda ($2B.b),Y
    cmp #opcode_localRedirect
    bne +
      ; redirect src to target of redirect
      iny
      lda ($2B.b),Y
      pha
        iny
        lda ($2B.b),Y
        sta $2C.b
      pla
      sta $2B.b
      
      ; decrement src so that Y will be in sync with position
      ; of both src and dst buffers
      lda $2B.b
      bne ++
        dec $2C.b
      ++:
      dec $2B.b
      
;      cly
;      rts
    +:
    
    rts
.ends*/

;==============================================================================
; new1
;==============================================================================

;===================================
; unbackgrounds
;===================================

.unbackground metabank_new1*sizeOfMetabank+newArea1LoadAddr+$0000 metabank_new1*sizeOfMetabank+newArea1LoadAddr+$1FFF

;===================================
; font
;===================================

.bank metabank_new1 slot std_slot
.section "new1: font 1" free
  fontStd:
    .incbin "out/font/font.bin"
.ends

.bank metabank_new1 slot std_slot
.section "new1: font 2" free
  fontNarrow:
    .incbin "out/font/font_narrow.bin"
.ends

.bank metabank_new1 slot std_slot
.section "new1: font 3" free
  fontNarrower:
    .incbin "out/font/font_narrower.bin"
.ends

.bank metabank_new1 slot std_slot
.section "new1: font 4" free
  fontWidthStd:
    .incbin "out/font/fontwidth.bin"
.ends

.bank metabank_new1 slot std_slot
.section "new1: font 5" free
  fontWidthNarrow:
    .incbin "out/font/fontwidth_narrow.bin"
.ends

.bank metabank_new1 slot std_slot
.section "new1: font 6" free
  fontWidthNarrower:
    .incbin "out/font/fontwidth_narrower.bin"
.ends

.bank metabank_new1 slot std_slot
.section "new1: font 7" free
  dteTable:
    .incbin "out/script/script_dictionary.bin"
.ends

;==============================================================================
; new2
;==============================================================================

;===================================
; unbackgrounds
;===================================

.unbackground metabank_new2*sizeOfMetabank+newArea2LoadAddr+$0000 metabank_new2*sizeOfMetabank+newArea2LoadAddr+$17FF

;==============================================================================
; new3
;==============================================================================

;===================================
; unbackgrounds
;===================================

.unbackground metabank_new3*sizeOfMetabank+newArea3LoadAddr+$0000 metabank_new3*sizeOfMetabank+newArea3LoadAddr+$1FFF

;==============================================================================
; subtitle engine
;==============================================================================

;===================================
; required defines
;===================================

.define newZpFreeReg $10
.define newZpScriptReg $12

.define fixedBank metabank_kernel
.define freeBank metabank_new3

.define fixedSlot std_slot
.define freeSlot std_slot

.define defaultSubtitleScriptPtr $0000
.define defaultSubtitleEngineOn $00

; TODO: not specific to this engine?
.define satMemBuf $2ACF
.define currentSpriteCount $9FFB
  
;.bank 0 slot 0
;.section "scene adv mem 1" free
;  ovlScene_subtitleEngineOn:
;    .db $00
;  ovlScene_rcrCropTargetsMultipleLines:
;    .db $00
;  ovlScene_subtitleDisplayOn:
;    .db $00
;.ends

; labels defined elsewhere:
; - ovlScene_font
; - ovlScene_fontWidthTable
; - newRcrOn

;===================================
; font
;===================================

.bank freeBank slot std_slot
.section "new3: subtitle font 1" free
  ovlScene_font:
    .incbin "out/font/font_scene.bin"
  ovlScene_fontWidthTable:
    .incbin "out/font/fontwidth_scene.bin"
.ends

;===================================
; 
;===================================

.bank fixedBank slot std_slot
.orga $46C5
.section "kernel: subtitle update 1" overwrite
  jsr doSubtitleUpdate
.ends

.bank fixedBank slot std_slot
.section "kernel: subtitle update 2" free
  doSubtitleUpdate:
    ; make up work
    jsr $4756
    
    ; don't run subtitle engine if not on.
    ; this is very important because otherwise we'll try to run code
    ; from new3 as soon as the vsync interrupt is turned on, which is before
    ; the extra code has even been loaded from disc.
    lda subtitleEngineOn.w
    bne +
      rts
    +:
    
;    jsr ovlScene_setUpStdBanks
;    jmp newSyncLogic
    tma #$40
    pha
      lda #newArea3MemPage
      tam #$40
      jsr newSyncLogic
    pla
    tam #$40
    rts
.ends

;===================================
; increment sync counter BEFORE
; intro voice clips, so that
; subtitles can start printing
; as the audio is loading
;===================================

/*.bank metabank_saveload slot std_slot
.orga $AF74
.section "saveload: pre-voice subtitle sync 1" overwrite
  jsr doDemoPreVoiceSyncUpdate
.ends

.bank metabank_saveload slot std_slot
.section "saveload: pre-voice subtitle sync 2" free
  doDemoPreVoiceSyncUpdate:
    doTrampolineCallNew3 incrementSyncVarCounterExt
    ; make up work
    jmp $84F5
.ends*/

.bank metabank_saveload slot std_slot
.orga $AE87
.section "saveload: pre-voice subtitle sync 1" overwrite
  jsr doDemoPreVoiceSyncUpdate
.ends

.bank metabank_saveload slot std_slot
.section "saveload: pre-voice subtitle sync 2" free
  doDemoPreVoiceSyncUpdate:
    ; check if next scene to be played has voice clip
    phy
      ; if byte 5 of specifier is not FF or FE, there is a voice clip
      ldy #5
      lda ($C7.b),Y
      cmp #$FF
      beq +
      cmp #$FE
      beq +
        doTrampolineCallNew3 incrementSyncVarCounterExt
      +:
    ply
    ; make up work
    jmp $7E9F
.ends

;===================================
; increment sync counter for
; e.g. intro voice clips
;===================================

.bank fixedBank slot std_slot
.orga $8361
.section "kernel: subtitle sync update 1" overwrite
  doTrampolineCallNew3 doSubtitleSyncCounterInc_demo
  jmp $836A
.ends

.bank freeBank slot std_slot
.section "new3: subtitle sync update 2" free
  doSubtitleSyncCounterInc_demo:
    ; make up work
    sta $FF.b
    stz $FA.b
    stz $FB.b
    ; this is a wrapped call to AD_PLAY -- clip has started playing
    ; once it returns
    jsr $47A9
    
    jmp incrementSyncVarCounterExt
  
  incrementSyncVarCounterExt:
    sei
      nop
      inc syncVar.w
      
      lda syncFrameCounter+0.w
      sta syncFrameCounterAtLastVarSync+0.w
      lda syncFrameCounter+1.w
      sta syncFrameCounterAtLastVarSync+1.w
    
      ; we actually want this value plus one, since we know that
      ; syncFrameCounter will be incremented at the next sync period
      ; and that incremented value is what the scripts check against
      inc syncFrameCounterAtLastVarSync+0.w
      bne +
        inc syncFrameCounterAtLastVarSync+1.w
      +:
    cli
    rts
.ends

;===================================
; increment sync counter for
; e.g. intro voice clips using AD_CPLAY
;===================================

.define masakadoAdpcmSectorH $00
.define masakadoAdpcmSectorM $98
.define masakadoAdpcmSectorL $66

.bank fixedBank slot std_slot
.orga $8389
.section "kernel: subtitle sync update AD_CPLAY 1" overwrite
  doTrampolineCallNew3 doSubtitleSyncCounterInc_demoAdCplay
  jmp $8392
.ends

.bank freeBank slot std_slot
.section "new3: subtitle sync update AD_CPLAY 2" free
  doSubtitleSyncCounterInc_demoAdCplay:
    ; make up work
    stz $FA.b
    lda #$0E
    sta $FF.b
    
    ; save adpcm src for later use
    lda _CL.b
    sta @checkCmd0+1.w
    lda _CH.b
    sta @checkCmd1+1.w
    lda _DL.b
    sta @checkCmd2+1.w
    
    ; make up work
    jsr AD_CPLAY
    
    ; HACK: check if this is masakado's voice clip for right before
    ; the final battle and trigger subtitles if so
    @checkCmd0:
    lda #$00
    cmp #masakadoAdpcmSectorH
    bne +
    @checkCmd1:
    lda #$00
    cmp #masakadoAdpcmSectorM
    bne +
    @checkCmd2:
    lda #$00
    cmp #masakadoAdpcmSectorL
    bne +
      stz subtitleEngineOn.w
      
      lda #<masakadoSubtitles
      sta subtitleScriptPtr+0.w
      lda #>masakadoSubtitles
      sta subtitleScriptPtr+1.w
      
      jsr resetForScriptStart
      
      inc subtitleEngineOn.w
    +:
    
    jmp incrementSyncVarCounterExt
.ends

;===================================
; stop subtitles if title demo cancelled
;===================================

.bank metabank_saveload slot std_slot
.orga $AEC0
.section "saveload: demo cancel subtitle stop 1" overwrite
  jsr doDemoCancelSubtitleStop
.ends

.bank metabank_saveload slot std_slot
.section "saveload: demo cancel subtitle stop 2" free
  doDemoCancelSubtitleStop:
    ; kill subtitles.
    ; note that it's okay to do this because cancelling the demo sequence
    ; also resets the scene count to zero, causing it to be reinitialized
    ; if the user idles long enough to let it run again (which also means
    ; the subtitles will be restarted).
    stz subtitleEngineOn.w
    
    ; make up work
    stz $3B27.w
    rts
.ends

;===================================
; stop subtitles if easter egg triggered
;===================================

.bank metabank_saveload slot std_slot
.orga $90DB
.section "saveload: title easter egg subtitle stop 1" overwrite
  jmp doEasterEggSubtitleStop
.ends

.bank metabank_saveload slot std_slot
.section "saveload: title easter egg subtitle stop 2" free
  doEasterEggSubtitleStop:
    ; kill subtitles
    stz subtitleEngineOn.w
    
    ; make up work
    jmp $8528
.ends

;===================================
; use new intro credits sprites
;===================================

.bank metabank_saveload slot std_slot
.orga $99E3
.section "saveload: new intro credits sprites 1" overwrite
  .dw newIntroCreditsSpriteDef
.ends

.bank metabank_saveload slot std_slot
.section "saveload: new intro credits sprites 2" free
  newIntroCreditsSpriteDef:
    .incbin "out/grp/intro_cred_sprite.bin"
.ends

;===================================
; 
;===================================

/*.bank fixedBank slot std_slot
.section "kernel: set up scene banks 1" free
  ovlScene_restoreOldBanks:
    @slot6:
    lda #$00
    tam #$40
    rts
.ends

.bank fixedBank slot std_slot
.section "kernel: set up scene banks 2" free
  ovlScene_setUpStdBanks:
    tma #$40
    sta ovlScene_restoreOldBanks@slot6+1.w
    
    lda #newArea3MemPage
    tam #$40
    rts
.ends*/

;===================================
; required includes
;===================================

.include "include/scene_adv_common.inc"

;===================================
; optional defines
;===================================

;.define includeScriptBoxFadeLogic 0

; why exactly did i make sprite flood mode enabled by default? whatever
.define noDefaultLineFloodMode 1

;===================================
; required extra routines
;===================================

;===================================
; patches to use engine
;===================================

; TODO
/*; vsync injection
.bank 0 slot 0
.orga $42CF
.section "scene adv vsync injection 1" overwrite
  jsr doAdvSceneCall
.ends*/

;===================================
; increment sync var for adpcm start
;===================================

; TODO
/*.bank 1 slot 0
.orga $8B13
.section "scene adv sync var 1" overwrite
  jmp doAcpdmSyncVarInc
.ends

.bank 0 slot 0
.section "scene adv sync var 2" free
  doAcpdmSyncVarInc:
;    doStdTrampolineCall doAcpdmSyncVarIncExt
    doStdTrampolineCall incrementSyncVarCounterExt
    ; make up work
    lda #$89
    rts
.ends*/

;===================================
; transfer generated sprites
;===================================

.bank metabank_kernel slot std_slot
.orga $4C4E
.section "kernel: use subtitle sprite demo 1" overwrite
  doTrampolineCallNew3 sendSubtitleSprites_demo
.ends

.bank freeBank slot std_slot
.section "new3: scene adv sprite generation 2" free
  sendSubtitleSprites_demo:
    jsr sendSubtitleSprites
    
    ; make up work
    ; send object sprites
    jsr $7596
    ; ?
    jsr $5397
    rts
  
  ; FIXME: not interrupt safe?
  sendSubtitleSprites:
    lda subtitleEngineOn.w
    beq @noSceneSprites
    lda subtitleDisplayOn.w
    beq @noSceneSprites
    lda currentSubtitleSpriteAttributeQueueSize.w
    beq @noSceneSprites
      ; save size to transfer instruction
;      sta @transferCmd+5.w
      lsr
      lsr
      lsr
      sta @loopSizeInstr+1.w
      
      ; set src address
      lda currentSubtitleSpriteAttributeQueuePtr+0.w
      sta @transferCmd+1.w
      lda currentSubtitleSpriteAttributeQueuePtr+1.w
      sta @transferCmd+2.w
      
      clx
      -:
;        @transferCmd:
;        lda $0000.w,X
;        sta satMemBuf.w,X
        ; copy sprite to src for showSprite data
        @transferCmd:
        tii $0000.w,$2217,8
        ; transfer to sat
        phx
          jsr showSprite
        plx
        ; update src pos to next sprite
        lda @transferCmd+1.w
        clc
        adc #8
        sta @transferCmd+1.w
        bcc +
          inc @transferCmd+2.w
        +:
        
        inx
        @loopSizeInstr:
        cpx #$00
        bne -
      
      ; return initial currentSpriteCount
      txa
;      lsr
;      lsr
;      lsr
    @noSceneSprites:
    rts
.ends

;===================================
; transfer generated sprites (overw)
;===================================

/*.bank metabank_overw slot std_slot
.orga $A584
.section "overw: use subtitle sprite overw 1" overwrite
  jsr sendSubtitleSprites_overw
.ends

.bank metabank_overw slot std_slot
.section "overw: use subtitle sprite overw 2" free
  sendSubtitleSprites_overw:
    lda subtitleEngineOn.w
    beq +
      doTrampolineCallNew3 sendSubtitleSprites
    +:
    
    ; make up work
    lda #$05
    clc
    rts
.ends*/

;===================================
; transfer generated sprites (masakado)
;===================================

; NOTE: the masakado pre-fight sequence loads special code from the disc
; specifically to play the complicated soul-spinning animation.
; the subtitle draw code is inserted directly into that, rather than into
; the generic overworld drawing code, because the complexity of the scene
; is enough that, in conjunction with the subtitle update code, the game
; sometimes lags, which -- due to the game entirely clearing the sprite table
; before writing new data to it -- will cause sprite drop-out.
; we want to make sure the subtitles are drawn first, to avoid having them
; disappear when lag occurs.
; (oh yeah, and even better reason: we want them to appear ON TOP OF the souls,
; not behind them.)
;
; this does have the drawback of causing the party's sprites to flicker
; occasionally, since they get drawn last. it'd be nice to make it so that
; they get drawn before the souls, but the game's sprite handling logic isn't
; really sufficient to do that (if we draw them first, they'll appear on top
; of the souls).

; there is apparently some sort of additional timing issue going on that's
; causing this not to work as intended. the game seems to do everything
; pretty much by the book:
; 1. clear the sat
; 2. write all sprites to the sat
; 3. schedule a sat->satb dma
; 4. wait for the vblank interrupt to set a flag
; the documentation claims that sat->satb dma will occur "at the beginning of
; the following vertical blanking period", and i would have thought that the
; procedure described here would result in the satb getting refreshed each
; frame that drawing completes.
; but apparently, there is a timing where if the sat->satb dma is scheduled
; too close to the start of vblank, it won't actually take effect until the
; following frame? or does the vsync interrupt actually not occur exactly at
; the start of vblank?
; whatever the case, this means that despite waiting for the next vsync
; interrupt, the sprite table may not be sent until the end of the NEXT
; frame, at which point we've already started writing new sprite data
; to the sat (and are probably not finished, resulting in an incomplete
; table being sent to the screen).
; 
; as a workaround, i've modified this routine to not clear the sprite table
; before drawing, instead clearing out any unused entries after writing
; all the sprites for the frame to the sat.
; this means that in the event of a "dropped" frame, some of the sprites will
; simply be copies of whatever they were last frame. the complexity of the
; special effect being done here should help mask any minor visual issues
; this causes.

/*.bank metabank_masakado slot std_slot
.orga $C073
.section "masakado: use subtitle sprite masakado 1" overwrite
  jsr sendSubtitleSprites_masakado
.ends

.bank metabank_overw slot std_slot
.section "overw: use subtitle sprite masakado 2" free
  sendSubtitleSprites_masakado:
    lda subtitleEngineOn.w
    beq +
      doTrampolineCallNew3 sendSubtitleSprites
    +:
    
    ; make up work (draw soul sprites)
    jmp $C11E
.ends*/

.bank metabank_masakado slot std_slot
.orga $C06D
.section "masakado: use subtitle sprite masakado 1" overwrite
  jmp sendSubtitleSprites_masakado
.ends

.define currentSpriteCount_game $2216

.bank metabank_overw slot std_slot
.section "overw: use subtitle sprite masakado 2" free
  sendSubtitleSprites_masakado:
    ; do not clear sprite table!
;    jsr $44A3
    
    ; make up work normally done by clearSpriteTable
    stz currentSpriteCount_game
    stz $33B5.w
    stz $36D5.w

    ; make up work
    jsr $C0CD
    
    ; draw subtitles if needed
    lda subtitleEngineOn.w
    beq +
      doTrampolineCallNew3 sendSubtitleSprites
    +:
    
    ; draw soul sprites
    jsr $C11E
    ; draw overworld sprites
    jsr $A580
    
    ; clear any unused entries of sprite table
    ; set mawr
    stz $F7.b
    st0 #$00
    ; count of sprites to clear = (0x40 - spriteCount)
    lda #$40
    sec
    sbc currentSpriteCount_game.w
    tax
    ; do nothing if table full
    lda currentSpriteCount_game.w
    cmp #$40
    bcs @noClear
      ; target = vram $0800 + (spriteCount * 4)
      asl
      asl
      sta $0002.w
      st2 #$08
      
      ; write to vram
      lda #$02
      sta $F7.b
      st0 #$02
      
      -:
        ; clear 8 bytes = 1 sprite
;        st1 #$00
;        st2 #$00
;        st1 #$00
;        st2 #$00
;        st1 #$00
;        st2 #$00
;        st1 #$00
;        st2 #$00
        tia @clearData,$0002,8
        
        dex
        bne -
    @noClear:
    
    ; resume normal logic
    jmp $C079
    
    @clearData:
    .rept 8
      .db $00
    .endr
.ends



/*.bank freeBank slot std_slot
.section "new3: scene adv sprite generation 3" free
  sendSubtitleSprites_overw_ext:
    jmp sendSubtitleSprites
.ends*/

;===================================
; flag vram writes
;===================================

; TODO: probably delete
/*.bank 1 slot 0
.orga $8502
.section "scene flag vram writes 1" overwrite
  doStdTrampolineCall doSceneVramWriteFlagStart_vramWrite
  nop
.ends

.bank 3 slot 0
.section "scene flag vram writes 2" free
  setBlockVramWrites:
    tma #$20
    pha
    lda #freeArea2MemPage
    tam #$20
      inc (blockVramWrites+$2000).w
    pla
    tam #$20
    rts
  
  clearBlockVramWrites:
    tma #$20
    pha
    lda #freeArea2MemPage
    tam #$20
      stz (blockVramWrites+$2000).w
    pla
    tam #$20
    rts
  
  doSceneVramWriteFlagStart_vramWrite:
    jsr setBlockVramWrites
    
    ; make up work
    lda ($12.b),Y
    sta $0018
    lda #$05
    rts
  
  doSceneVramWriteFlagEndExt_vramWrite:
    jsr clearBlockVramWrites
    
    ; make up work
    lda #$88
    rts
  
  doSceneVramWriteFlagStart_tilemapWrite:
    jsr setBlockVramWrites
    
    ; make up work
    lda #$05
    sta $6A.b
    sta $0000.w
    rts
  
  doSceneVramWriteFlagEndExt_tilemapWrite:
    jsr clearBlockVramWrites
    
    ; make up work
    lda #$8B
    rts
.ends

.bank 1 slot 0
.orga $85D1
.section "scene flag vram writes 3" overwrite
  jmp doSceneVramWriteFlagEnd_vramWrite
.ends

.bank 0 slot 0
.section "scene flag vram writes 4" free
  doSceneVramWriteFlagEnd_vramWrite:
    doStdTrampolineCall doSceneVramWriteFlagEndExt_vramWrite
    rts
.ends*/

;===================================
; flag tilemap writes
;===================================

; TODO: probably delete
/*.bank 1 slot 0
.orga $8682
.section "scene flag tilemap writes 1" overwrite
  doStdTrampolineCall doSceneVramWriteFlagStart_tilemapWrite
  nop
  nop
.ends

.bank 1 slot 0
.orga $86FD
.section "scene flag tilemap writes 2" overwrite
  jmp doSceneVramWriteFlagEnd_tilemapWrite
.ends

.bank 0 slot 0
.section "scene flag tilemap writes 3" free
  doSceneVramWriteFlagEnd_tilemapWrite:
    doStdTrampolineCall doSceneVramWriteFlagEndExt_tilemapWrite
    rts
.ends*/

;===================================
; set up subtitle script when
; starting adv (if needed)
;===================================

; TODO: probably delete
/*.bank 1 slot 0
.orga $9541
.section "start adv subs if needed 1" SIZE 5 overwrite
  doStdTrampolineCall doAdvSubsSetup_makeup
;  jmp $954B
.ends

.bank 3 slot 0
.section "start adv subs if needed 2" free
  doAdvSubsSetup_makeup:
    jsr doAdvSubsSetup
    
    ; make up work
    lda #$10
    sta $333E.w
;    lda #$01
;    sta $3330.w
    rts
    
    
  doAdvSubsSetup:
    ; load in sub bank
    tma #$20
    pha
    lda #advSceneBaseBank
    tam #$20
      ; check for presence of magic bytes at end of bank
      lda advSceneBlockStartPtr+advScenePresentMarkerBlockOffset+0.w
      cmp #advScenePresentMarkerA
      bne @noSubs
      lda advSceneBlockStartPtr+advScenePresentMarkerBlockOffset+1.w
      cmp #advScenePresentMarkerB
      bne @noSubs
        ; set script pointer
        ; NOTE: we assume subtitle engine is off at this point;
        ; otherwise, this would need interrupts disabled to be 100% safe
        lda advSceneBlockStartPtr+advScenePointerBlockOffset+0.w
        sta subtitleScriptPtr+0.w
        lda advSceneBlockStartPtr+advScenePointerBlockOffset+1.w
        sta subtitleScriptPtr+1.w
        
        ; reset for new script
        jsr resetForScriptStart
        
        inc subtitleEngineOn.w
      @noSubs:
    pla
    tam #$20
    rts
.ends*/

;===================================
; actual engine
;===================================

.include "include/scene_adv.inc"

;===================================
; subtitles
;===================================

.define extraSubDisplaylength 0.200

.bank metabank_new3 slot std_slot
.section "new3: subtitle data 1" free
  introDemoSubtitles:
    ;=====
    ; init
    ;=====
    
    SCENE_setUpAutoPlace $1B0 $20
    
    cut_setPalette $08
    
    cut_writeMem newArea3MemPage subtitleBaseY defaultSubtitleBaseY
  
    ;=====
    ; wait until safe start point
    ;=====
    
;    cut_waitForFrameMinSec 0 2.500
    
;    cut_waitForFrameMinSec 1 2.500
  
    ;=====
    ; data
    ;=====
  
    ;=====
    ; 1
    ;=====
    
    SYNC_varTime 1 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_1.bin"
    SYNC_varTime 2 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 1.700+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 3
    ;=====
    
    SYNC_varTime 3 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_3.bin"
    SYNC_varTime 4 $00
    cut_waitForFrameMinSec 0 2.700
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.664+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 5
    ;=====
    
    SYNC_varTime 5 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_5.bin"
    SYNC_varTime 6 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_6.bin"
    cut_waitForFrameMinSec 0 5.049
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 6.905+0.150+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 7
    ;=====
    
    SYNC_varTime 7 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_7.bin"
    SYNC_varTime 8 $00
    cut_waitForFrameMinSec 0 0.152
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_8.bin"
    cut_waitForFrameMinSec 0 1.926
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.098+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 9
    ;=====
    
    SYNC_varTime 9 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_9.bin"
    SYNC_varTime 10 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 3.411+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 11
    ;=====
    
    SYNC_varTime 11 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_11.bin"
    SYNC_varTime 12 $00
    cut_waitForFrameMinSec 0 1.240
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 4.056+extraSubDisplaylength
    cut_subsOff
  
    ;======================
    ; ** RESET TO TITLE **
    ;======================
  
    ;=====
    ; 13
    ;=====
    
    SYNC_varTime 13 $00
    SCENE_startNewStringAuto
    ; "ziria! it is for this"
    SCENE_addString "out/script/subtitle/subtitle_13.bin"
    SYNC_varTime 14 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.563+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 15
    ;=====
    
    SYNC_varTime 15 $00
    SCENE_startNewStringAuto
    ; "so sleepy"
    SCENE_addString "out/script/subtitle/subtitle_15.bin"
    SYNC_varTime 16 $00
    cut_waitForFrameMinSec 0 1.928
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.556+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 17
    ;=====
    
    ; sound effect
  
    ;=====
    ; 19
    ;=====
    
    SYNC_varTime 19 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_19.bin"
    SYNC_varTime 20 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 4.001+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 21
    ;=====
    
    SYNC_varTime 21 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_21.bin"
    SYNC_varTime 22 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.714+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 23
    ;=====
    
    SYNC_varTime 23 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_23.bin"
    SYNC_varTime 24 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 3.146+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 25
    ;=====
    
    ; sound effect
  
    ;======================
    ; ** RESET TO TITLE **
    ;======================
  
    ;=====
    ; 27
    ;=====
    
    SYNC_varTime 27 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_27.bin"
    SYNC_varTime 28 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 3.714+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 29
    ;=====
    
    SYNC_varTime 29 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_29.bin"
    SYNC_varTime 30 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_30.bin"
;    cut_waitForFrameMinSec 0 2.639
    cut_waitForFrameMinSec 0 5.308
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 7.668+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 31
    ;=====
    
    SYNC_varTime 31 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_31.bin"
    SYNC_varTime 32 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.977+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 33
    ;=====
    
    SYNC_varTime 33 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_33.bin"
    SYNC_varTime 34 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 3.263+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 35
    ;=====
    
    SYNC_varTime 35 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_35.bin"
    SYNC_varTime 36 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 1.083+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 37
    ;=====
    
    SYNC_varTime 37 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_37.bin"
    SYNC_varTime 38 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_38.bin"
    cut_waitForFrameMinSec 0 6.052
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 8.922+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 39
    ;=====
    
    SYNC_varTime 39 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_39.bin"
    SYNC_varTime 40 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_40.bin"
    cut_waitForFrameMinSec 0 4.259
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 6.657+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 41
    ;=====
    
    SYNC_varTime 41 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_41.bin"
    SYNC_varTime 42 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_42.bin"
    cut_waitForFrameMinSec 0 4.328+0.200
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 6.898+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 43
    ;=====
    
    SYNC_varTime 43 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_43.bin"
    SYNC_varTime 44 $00
    cut_waitForFrameMinSec 0 1.579
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 7.398+extraSubDisplaylength
    cut_subsOff
  
    ;======================
    ; ** RESET TO TITLE **
    ;======================
  
    ;=====
    ; 45
    ;=====
    
    SYNC_varTime 45 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_45.bin"
    SYNC_varTime 46 $00
    cut_swapAndShowBuf
    
;    cut_waitForFrameMinSec 0 2.759
    
    cut_waitForFrameMinSec 0 5.706+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 47
    ;=====
    
    SYNC_varTime 47 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_47.bin"
    SYNC_varTime 48 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 4.195+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 49
    ;=====
    
    SYNC_varTime 49 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_49.bin"
    SYNC_varTime 50 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_50.bin"
    cut_waitForFrameMinSec 0 4.458
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 8.153+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 51
    ;=====
    
    SYNC_varTime 51 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_51.bin"
    SYNC_varTime 52 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.014+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 53
    ;=====
    
    SYNC_varTime 53 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_53.bin"
    SYNC_varTime 54 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.978+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 55
    ;=====
    
    SYNC_varTime 55 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_55.bin"
    SYNC_varTime 56 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 4.368+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 57
    ;=====
    
    SYNC_varTime 57 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_57.bin"
    SYNC_varTime 58 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_58.bin"
;    cut_waitForFrameMinSec 0 3.740
    cut_waitForFrameMinSec 0 8.243
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 10.784+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 59
    ;=====
    
    ; sound effect
  
    ;======================
    ; ** RESET TO TITLE **
    ;======================
  
    ;=====
    ; 61
    ;=====
    
    SYNC_varTime 61 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_61.bin"
    SYNC_varTime 62 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_62.bin"
    cut_waitForFrameMinSec 0 5.180
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 9.856+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 63
    ;=====
    
    SYNC_varTime 63 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_63.bin"
    SYNC_varTime 64 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_64.bin"
    cut_waitForFrameMinSec 0 3.376
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 9.183+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 65
    ;=====
    
    SYNC_varTime 65 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_65.bin"
    SYNC_varTime 66 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.202+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 67
    ;=====
    
    SYNC_varTime 67 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_67.bin"
    SYNC_varTime 68 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_68.bin"
    cut_waitForFrameMinSec 0 3.661
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.736+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 69
    ;=====
    
    SYNC_varTime 69 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_69.bin"
    SYNC_varTime 70 $00
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 5.018+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 71
    ;=====
    
    SYNC_varTime 71 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_71.bin"
    SYNC_varTime 72 $00
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_72.bin"
    cut_waitForFrameMinSec 0 2.691
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 6.975+extraSubDisplaylength
    cut_subsOff
  
    ;=====
    ; 73
    ;=====
    
    SYNC_varTime 73 $00
    SCENE_startNewStringAuto
    ; ""
    SCENE_addString "out/script/subtitle/subtitle_73.bin"
    SYNC_varTime 74 $00
    cut_swapAndShowBuf
    
;    cut_waitForFrameMinSec 0 5.864
    cut_waitForFrameMinSec 0 5.982
    cut_subsOff
  
    ;======================
    ; ** RESET TO TITLE **
    ;======================
    
    cut_terminator
  
  masakadoSubtitles:
    ;=====
    ; init
    ;=====
    
    SCENE_setUpAutoPlace $1B0 $20
    
    cut_setPalette $08
    
    cut_writeMem newArea3MemPage subtitleBaseY defaultSubtitleBaseY+16
    
    SCENE_startNewStringAuto
    ; "i am masakado"
    SCENE_addString "out/script/subtitle/subtitle_75.bin"
    ; pretty sure this is the correct time, but something's throwing
    ; things off and we have to put it earlier for proper sync...
;    cut_waitForFrameMinSec 0 4.765
    cut_waitForFrameMinSec 0 4.765-0.483
    cut_swapAndShowBuf
    
    SCENE_startNewStringAuto
    ; "the doom of jipang"
    SCENE_addString "out/script/subtitle/subtitle_76.bin"
    cut_waitForFrameMinSec 0 6.909
    cut_swapAndShowBuf
    
    cut_waitForFrameMinSec 0 9.958+extraSubDisplaylength
    cut_subsOff
    
    cut_terminator
    
.ends

;==============================================================================
; credits
;==============================================================================

.unbackground metabank_intro*sizeOfMetabank+$BEC0 metabank_intro*sizeOfMetabank+$BFFF
.unbackground metabank_credits*sizeOfMetabank+$C000 metabank_credits*sizeOfMetabank+$CFFF

;===================================
; 
;===================================

.define cred_setCreditsSpritePackedFlagsTo3901 $91EA
.define cred_setCreditsSpriteWidthFlag $91F1
.define cred_sendPendingCreditsSprite $91FA
.define cred_readStrPositionData $928D
.define cred_findEmptyCreditsSpriteSlot $92B5
.define cred_setPrintVramTargetBasedOnX $92C4

.define cred_srcPtr_zp $C3

.define cred_nextSpriteBuf $33C9
.define cred_charVramDstLo $38FD
.define cred_charVramDstHi $38FE
.define cred_creditsSpriteCoarseX $38FF
.define cred_creditsSpriteCoarseY $3900
.define cred_creditsSpriteDispTime $3902

.define cred_op_terminator $FF

.define tallFontHeight 16
.define tallFontWidth 8
.define newCreditsPrintBuffer_size $200
.define creditsFontShiftBuffer_size ((tallFontWidth*2)*tallFontHeight)/8
.define cred_nextSpriteBuf_size 32
; original game specifies x in units of 8; we don't want that
;.define creditsSpriteXAdvanceW 4
.define creditsSpriteXAdvanceW 32

.bank metabank_intro slot std_slot
.orga $90D0
.section "intro: new credits printing 1" SIZE $54 overwrite
  printNewCreditsString:
    ; reset buffer
    stz newCreditsPrintBuffer_currentW.w
    doTrampolineCallNew2 clearNewCreditsPrintBuffer
    
    jsr cred_readStrPositionData
    
    @printLoop:
      ; fetch next byte
      lda (cred_srcPtr_zp.b)
      ; done if terminator
      cmp #cred_op_terminator
      beq @finish
      
      ; print char
      doTrampolineCallNew2 printNewCreditsChar
      
      ; ++src
      inc cred_srcPtr_zp+0.b
      bne +
        inc cred_srcPtr_zp+1.b
      +:
      
      bra @printLoop
    
    @finish:
    ; skip terminator
    inc cred_srcPtr_zp+0.b
    bne +
      inc cred_srcPtr_zp+1.b
    +:
    
    ; send sprites
    doTrampolineCallNew2 sendCreditsSprites
    
    rts
.ends

.bank metabank_intro slot std_slot
.section "intro: new credits printing 3" free
  newCreditsPrintBuffer_currentW:
    .db $00
  creditsFontShiftBuffer:
    .ds ((tallFontWidth*2)*tallFontHeight)/8,$00
.ends

.bank metabank_new2 slot std_slot
.section "new2: new credits printing 4" free
  newCreditsPrintBuffer:
    ; contains enough space for a 256x16 1bpp area,
    ; arranged as 16 1bpp sprites with two bytes per line
    .ds newCreditsPrintBuffer_size,$00
  ; width of the content in the print buffer
  
  fontTall:
    .incbin "out/font/font_tall.bin"
  fontWidthTall:
    .incbin "out/font/fontwidth_tall.bin"
  
  ; A = character index
  clearNewCreditsPrintBuffer:
    tai zeroWord,newCreditsPrintBuffer,newCreditsPrintBuffer_size
;    tai zeroWord,creditsFontShiftBuffer,creditsFontShiftBuffer_size
    rts
  
  zeroWord:
    .dw $0000
    
  sendCreditsSprites:
    lda #<newCreditsPrintBuffer
    sta @spriteCopyCmd+1.w
    lda #>newCreditsPrintBuffer
    sta @spriteCopyCmd+2.w
    @spriteSendLoop:
      jsr cred_findEmptyCreditsSpriteSlot
      bcc @done
      jsr cred_setPrintVramTargetBasedOnX
      
      ; ?
      lda #$01
      sta $39C3.w,X
      lda cred_creditsSpriteDispTime.w
      sta $3A83.w,X
      
      ; copy 2 sprites
      ; (regardless of input size; i'm hoping we won't need to bother fully
      ; optimizing this as the original code does)
      ldy #2
      -:
        phy
          @spriteCopyCmd:
          ; copy next sprite to buffer
          tii $0000,cred_nextSpriteBuf,cred_nextSpriteBuf_size
          ; send to vram (with color conversions)
          jsr cred_sendPendingCreditsSprite
          jsr cred_setCreditsSpritePackedFlagsTo3901
          
          ; src += pattern size
          lda @spriteCopyCmd+1.w
          clc
          adc #cred_nextSpriteBuf_size
          sta @spriteCopyCmd+1.w
          bcc +
            inc @spriteCopyCmd+2.w
          +:
          
          ; move to next vram pos
          lda cred_charVramDstLo.w
          clc
          adc #$40
          sta cred_charVramDstLo.w
          bcc +
            inc cred_charVramDstHi.w
          +:
        ply
        dey
        bne -
      
      ; adjust output x-pos
      lda cred_creditsSpriteCoarseX.w
      clc
;      adc #$04
      adc #creditsSpriteXAdvanceW
      sta cred_creditsSpriteCoarseX.w
      
      ; width -= transferSize
      ; if remaining width <= 16, do not set sprite double-width flag
      lda newCreditsPrintBuffer_currentW.w
      cmp #17
      bcc +
        pha
          jsr cred_setCreditsSpriteWidthFlag
        pla
      +:
      sec
      sbc #32
      sta newCreditsPrintBuffer_currentW.w
      ; loop while content remains
      beq @done
      bcs @spriteSendLoop
      
    @done:
    rts
  
  ; A = character index
  printNewCreditsChar:
    phx
      ; convert from raw codepoint to font index
      sec
      sbc #fontBaseOffset
      sta @targetCharLoadCmd+1.w
      
      ; save credits pointer while we use its memory for dst ptr
      lda cred_srcPtr_zp+0.b
      pha
      lda cred_srcPtr_zp+1.b
      pha
        lda newCreditsPrintBuffer_currentW.w
        
        ; save fine pixel offset
        pha
          and #$7
          sta @fontShiftCmd+1.w
        pla
        
        ; compute buffer coarse dstpos:
        ; bufferPos + ((bufferW / 16) * 32) + ((bufferW & 0x08) ? 1 : 0)
        pha
          stz @dstHighByteCmd+1.w
          asl
          rol @dstHighByteCmd+1.w
          and #$E0
          
          clc
          adc #<newCreditsPrintBuffer
          sta cred_srcPtr_zp+0.b
          @dstHighByteCmd:
          lda #$00
          adc #>newCreditsPrintBuffer
          sta cred_srcPtr_zp+1.b
        pla
        ; if bit 3 of width is set, we are targeting the right side of
        ; the sprite and need to add 1 to the dst offset
        and #$08
        pha
          beq +
            inc cred_srcPtr_zp+0.b
            bne +
              inc cred_srcPtr_zp+1.b
          +:
          
          @targetCharLoadCmd:
          lda #$00
          ; add width of new character to current buffer width
          tax
            lda fontWidthTall.w,X
            clc
            adc newCreditsPrintBuffer_currentW.w
            sta newCreditsPrintBuffer_currentW.w
          txa
          
          ; compute target position in font data
          ; (16 bytes per character)
          stz @fontOffsetHiCmd+1.w
          asl
          rol @fontOffsetHiCmd+1.w
          asl
          rol @fontOffsetHiCmd+1.w
          asl
          rol @fontOffsetHiCmd+1.w
          asl
          rol @fontOffsetHiCmd+1.w
          
          clc
          adc #<fontTall
          sta @fontCopyCmd+1.w
          @fontOffsetHiCmd:
          lda #$00
          adc #>fontTall
          sta @fontCopyCmd+2.w
          
          ; copy target character's font data to buffer,
          ; right-shifting by the needed number of pixels
          ; to match the fine offset in dst
          clx
          cly
          @fontToBufferCopyLoopOuter:
            stz creditsFontShiftBuffer+1.w,X
            @fontCopyCmd:
            lda $0000.w,Y
            
            phy
              @fontShiftCmd:
              ldy #$00
              beq @fontToBufferCopyLoopInner_done
              @fontToBufferCopyLoopInner:
                lsr
                ror creditsFontShiftBuffer+1.w,X
                dey
                bne @fontToBufferCopyLoopInner
              @fontToBufferCopyLoopInner_done:
              sta creditsFontShiftBuffer+0.w,X
            ply
            
            inx
            inx
            iny
            cpy #tallFontHeight
            bne @fontToBufferCopyLoopOuter
          
          ; compose left half of src buffer with dst and write to dst
          
;          clx
          cly
          @copyLoop1:
            ; fetch next line of character data
            lda creditsFontShiftBuffer.w,Y
            ; combine with next line of output buffer
            ora (cred_srcPtr_zp.b),Y
            sta (cred_srcPtr_zp.b),Y
            
            ; move to next line of src
;            inx
;            inx
            ; move to next line of dst
            iny
            iny
            
            cpy #(tallFontHeight*2)
            bne @copyLoop1
        
          ; advance dst to next half.
          ; difference is 1 if we were previously targeting the left side
          ; and 31 if previously targeting the right side.
          
          inc cred_srcPtr_zp+0.b
          bne +
            inc cred_srcPtr_zp+1.b
          +:
        ; restore bit 3 of bufferW
        pla
        ; if bit 3 set, we are on the right side of the pattern
        ; and must add 30 in order to advance to the left side
        ; of the next pattern
;        and #$08
        beq +
          lda #30
          clc
          adc cred_srcPtr_zp+0.b
          sta cred_srcPtr_zp+0.b
          bcc +
            inc cred_srcPtr_zp+1.b
        +:
        
        ; copy right half of src buffer to dst
        ; TODO: can skip this if there was no shift
        
/*        ; advance src to right half
        ldx #$01
        cly
        @copyLoop2:
          ; fetch next line of character data
          @fontCopyCmd2:
          lda $0000.w,X
          ; combine with next line of output buffer
          ora (cred_srcPtr_zp.b),Y
          sta (cred_srcPtr_zp.b),Y
          
          ; move to next line of src
          inx
          inx
          ; move to next line of dst
          iny
          iny
          
          cpy #(tallFontHeight*2)
          bne @copyLoop2*/
        
        ; advance src to right half
;        ldx #$01
        cly
        @copyLoop2:
          ; fetch next line of character data
          lda creditsFontShiftBuffer+1.w,Y
          ; combine with next line of output buffer
          sta (cred_srcPtr_zp.b),Y
          
          ; move to next line of src
;          inx
;          inx
          ; move to next line of dst
          iny
          iny
          
          cpy #(tallFontHeight*2)
          bne @copyLoop2
        
      pla
      sta cred_srcPtr_zp+1.b
      pla
      sta cred_srcPtr_zp+0.b
    plx
    rts
.ends

.bank metabank_intro slot std_slot
.orga $9162
.section "intro: new credits printing 5" overwrite
  ; use new width format for sjis strings
;  adc #$04
  adc #creditsSpriteXAdvanceW
.ends

.bank metabank_intro slot std_slot
.orga $94C5
.section "intro: new credits printing 6" overwrite
  ; do not multiply x-pos by 8
  jmp $94D1
.ends

;===================================
; credits macros
;===================================

.macro addCreditsDelay ARGS timeSec
  .db timeSec
.endm

.macro makeCreditsString ARGS timeSec, x, y, filename
  .db (timeSec|$80)
  .db x
  .db (y/8)
  .incbin filename
.endm

.macro makeCreditsStringJp ARGS timeSec, x, y, filename
  .db (timeSec|$40)
  .db x
  .db (y/8)
  .incbin filename
.endm

;===================================
; credits
;===================================

.include "out/script/credits/centerwidths.inc"

.bank metabank_credits slot std_slot
.orga $C000
.section "credits: new credits data" overwrite
;  .db $FF
  
  addCreditsDelay 2

;  makeCreditsString 4, $70, $30, "out/script/credits/str-0x4.bin"
;  makeCreditsString 4, $60, $50, "out/script/credits/str-0xC.bin"
  makeCreditsString 4, credCenterX_str_0x4, $30, "out/script/credits/str-0x4.bin"
  makeCreditsString 4, credCenterX_str_0xC, $50, "out/script/credits/str-0xC.bin"
;  makeCreditsStringJp 4, $70, $30, "rsrc_raw/credits/str-0x4.bin"
;  makeCreditsStringJp 4, $60, $50, "rsrc_raw/credits/str-0xC.bin"
  addCreditsDelay 5

  ; "executive producer"
;  makeCreditsString 4, $58, $30, "out/script/credits/str-0x19.bin"
;  makeCreditsString 4, $60, $50, "out/script/credits/str-0x27.bin"
  makeCreditsString 4, credCenterX_str_0x19, $30, "out/script/credits/str-0x19.bin"
  makeCreditsString 4, credCenterX_str_0x27, $50, "out/script/credits/str-0x27.bin"
  addCreditsDelay 5

  ; "producers"
  makeCreditsString 8, $10+$10, $30, "out/script/credits/str-0x34.bin"
  makeCreditsString 4, $A0-$8, $30, "out/script/credits/str-0x46.bin"
;  makeCreditsStringJp 4, $A0, $30, "rsrc_raw/credits/str-0x46.bin"
  addCreditsDelay 4

  makeCreditsString 4, $A0-$8, $30, "out/script/credits/str-0x53.bin"
;  makeCreditsStringJp 4, $A0, $30, "rsrc_raw/credits/str-0x53.bin"
  makeCreditsString 4, $A0-$8, $50, "out/script/credits/str-0x5F.bin"
;  makeCreditsStringJp 4, $A0, $50, "rsrc_raw/credits/str-0x5F.bin"
  addCreditsDelay 5

  ; "original work"
  makeCreditsString 4, credCenterX_str_0x6C, $30, "out/script/credits/str-0x6C.bin"
  makeCreditsString 4, credCenterX_str_0x74, $50, "out/script/credits/str-0x74.bin"
  makeCreditsString 4, credCenterX_str_0x86, $60, "out/script/credits/str-0x86.bin"
  addCreditsDelay 5

  ; "script"
  makeCreditsString 4, credCenterX_str_0xA3, $30, "out/script/credits/str-0xA3.bin"
  makeCreditsString 4, credCenterX_str_0xAB, $50, "out/script/credits/str-0xAB.bin"
  addCreditsDelay 5

  ; "music producer"
  makeCreditsString 4, credCenterX_str_0xBC, $30, "out/script/credits/str-0xBC.bin"
  makeCreditsString 4, credCenterX_str_0xD0, $50, "out/script/credits/str-0xD0.bin"
  addCreditsDelay 5

  ; "voice acting"
  makeCreditsString 20, credCenterX_str_0xDD, $20, "out/script/credits/str-0xDD.bin"
  makeCreditsString 4, credCenterX_str_0xE9, $38, "out/script/credits/str-0xE9.bin"
  makeCreditsString 4, credCenterX_str_0xF5, $48, "out/script/credits/str-0xF5.bin"
  makeCreditsString 4, credCenterX_str_0x101, $58, "out/script/credits/str-0x101.bin"
  makeCreditsString 4, credCenterX_str_0x10D, $68, "out/script/credits/str-0x10D.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x11A, $38, "out/script/credits/str-0x11A.bin"
  makeCreditsString 4, credCenterX_str_0x126, $48, "out/script/credits/str-0x126.bin"
  makeCreditsString 4, credCenterX_str_0x132, $58, "out/script/credits/str-0x132.bin"
  makeCreditsString 4, credCenterX_str_0x13E, $68, "out/script/credits/str-0x13E.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x14B, $38, "out/script/credits/str-0x14B.bin"
  makeCreditsString 4, credCenterX_str_0x157, $48, "out/script/credits/str-0x157.bin"
  makeCreditsString 4, credCenterX_str_0x163, $58, "out/script/credits/str-0x163.bin"
  makeCreditsString 4, credCenterX_str_0x16F, $68, "out/script/credits/str-0x16F.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x17C, $38, "out/script/credits/str-0x17C.bin"
  makeCreditsString 4, credCenterX_str_0x188, $48, "out/script/credits/str-0x188.bin"
  makeCreditsString 4, credCenterX_str_0x194, $58, "out/script/credits/str-0x194.bin"
  makeCreditsString 4, credCenterX_str_0x1A0, $68, "out/script/credits/str-0x1A0.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x1AF, $38, "out/script/credits/str-0x1AF.bin"
  makeCreditsString 4, credCenterX_str_0x1BB, $48, "out/script/credits/str-0x1BB.bin"
  makeCreditsString 4, credCenterX_str_0x1C7, $58, "out/script/credits/str-0x1C7.bin"
  makeCreditsString 4, credCenterX_str_0x1D3, $68, "out/script/credits/str-0x1D3.bin"
  makeCreditsString 4, credCenterX_str_0x1DF, $78, "out/script/credits/str-0x1DF.bin"
  addCreditsDelay 5

  ; "guest performances"
  makeCreditsString 4, credCenterX_str_0x1EC, $20, "out/script/credits/str-0x1EC.bin"
  makeCreditsString 4, credCenterX_str_0x1F8, $38, "out/script/credits/str-0x1F8.bin"
  makeCreditsString 4, credCenterX_str_0x206, $50, "out/script/credits/str-0x206.bin"
  makeCreditsString 4, credCenterX_str_0x214, $68, "out/script/credits/str-0x214.bin"
  addCreditsDelay 5

  ; "art director"
  makeCreditsString 4, credCenterX_str_0x221, $30, "out/script/credits/str-0x221.bin"
  makeCreditsString 4, credCenterX_str_0x22D, $50, "out/script/credits/str-0x22D.bin"
  addCreditsDelay 5

  ; "graphic supervisors"
  makeCreditsString 4, credCenterX_str_0x23C, $20, "out/script/credits/str-0x23C.bin"
  makeCreditsString 4, credCenterX_str_0x250, $38, "out/script/credits/str-0x250.bin"
  makeCreditsString 4, credCenterX_str_0x25C, $50, "out/script/credits/str-0x25C.bin"
  addCreditsDelay 5

  ; "artwork"
  makeCreditsString 4, $40-$20, $20, "out/script/credits/str-0x269.bin"
  makeCreditsString 4, $90, $20, "out/script/credits/str-0x271.bin"
  makeCreditsString 4, $90, $38, "out/script/credits/str-0x27D.bin"
  makeCreditsString 4, $90, $50, "out/script/credits/str-0x289.bin"
  addCreditsDelay 5

  ; "backgrounds"
  makeCreditsString 4, credCenterX_str_0x298, $30, "out/script/credits/str-0x298.bin"
  makeCreditsString 4, credCenterX_str_0x2A0, $50, "out/script/credits/str-0x2A0.bin"
  addCreditsDelay 5

  ; "music director"
  makeCreditsString 4, credCenterX_str_0x2B7, $30, "out/script/credits/str-0x2B7.bin"
  makeCreditsString 4, credCenterX_str_0x2C3, $50, "out/script/credits/str-0x2C3.bin"
  addCreditsDelay 5

  ; "music"
  makeCreditsString 12, credCenterX_str_0x2D0, $20, "out/script/credits/str-0x2D0.bin"
  makeCreditsString 4, credCenterX_str_0x2D8, $48, "out/script/credits/str-0x2D8.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x2E7, $38, "out/script/credits/str-0x2E7.bin"
  makeCreditsString 4, credCenterX_str_0x2F3, $50, "out/script/credits/str-0x2F3.bin"
  makeCreditsString 4, credCenterX_str_0x2FF, $68, "out/script/credits/str-0x2FF.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x30C, $38, "out/script/credits/str-0x30C.bin"
  makeCreditsString 4, credCenterX_str_0x318, $50, "out/script/credits/str-0x318.bin"
  addCreditsDelay 5

  ; "sound creators"
  makeCreditsString 4, credCenterX_str_0x325, $20, "out/script/credits/str-0x325.bin"
  makeCreditsString 4, credCenterX_str_0x339, $38, "out/script/credits/str-0x339.bin"
  makeCreditsString 4, credCenterX_str_0x345, $50, "out/script/credits/str-0x345.bin"
  addCreditsDelay 5

  ; "assistant directors"
  makeCreditsString 4, $30-$18, $20, "out/script/credits/str-0x352.bin"
  makeCreditsString 4, $90, $20, "out/script/credits/str-0x35C.bin"
  makeCreditsString 4, $90, $38, "out/script/credits/str-0x368.bin"
  makeCreditsString 4, $90, $50, "out/script/credits/str-0x374.bin"
  addCreditsDelay 5

  ; "system programmers"
  makeCreditsString 4, credCenterX_str_0x381, $20, "out/script/credits/str-0x381.bin"
  makeCreditsString 4, credCenterX_str_0x399, $48, "out/script/credits/str-0x399.bin"
  makeCreditsString 4, credCenterX_str_0x3A5, $60, "out/script/credits/str-0x3A5.bin"
  addCreditsDelay 5

  ; "game programmers"
  makeCreditsString 4, credCenterX_str_0x3B2, $20, "out/script/credits/str-0x3B2.bin"
  makeCreditsString 4, credCenterX_str_0x3C8, $38, "out/script/credits/str-0x3C8.bin"
  makeCreditsString 4, credCenterX_str_0x3D4, $50, "out/script/credits/str-0x3D4.bin"
  addCreditsDelay 5

  ; "sound programmers"
  makeCreditsString 4, credCenterX_str_0x3E1, $20, "out/script/credits/str-0x3E1.bin"
  makeCreditsString 4, credCenterX_str_0x3F9, $38, "out/script/credits/str-0x3F9.bin"
  makeCreditsString 4, credCenterX_str_0x405, $48, "out/script/credits/str-0x405.bin"
  makeCreditsString 4, credCenterX_str_0x411, $58, "out/script/credits/str-0x411.bin"
  makeCreditsString 4, credCenterX_str_0x41D, $68, "out/script/credits/str-0x41D.bin"
  addCreditsDelay 5

  ; "map designers"
  makeCreditsString 4, credCenterX_str_0x42A, $20, "out/script/credits/str-0x42A.bin"
  makeCreditsString 4, credCenterX_str_0x43E, $38, "out/script/credits/str-0x43E.bin"
  makeCreditsString 4, credCenterX_str_0x44A, $50, "out/script/credits/str-0x44A.bin"
  addCreditsDelay 5

  ; "animation designers"
  makeCreditsString 4, credCenterX_str_0x457, $20, "out/script/credits/str-0x457.bin"
  makeCreditsString 4, credCenterX_str_0x473, $38, "out/script/credits/str-0x473.bin"
  makeCreditsString 4, credCenterX_str_0x47F, $48, "out/script/credits/str-0x47F.bin"
  makeCreditsString 4, credCenterX_str_0x48B, $58, "out/script/credits/str-0x48B.bin"
  makeCreditsString 4, credCenterX_str_0x497, $68, "out/script/credits/str-0x497.bin"
  addCreditsDelay 5

  ; "animation"
  makeCreditsString 12, credCenterX_str_0x4A4, $20, "out/script/credits/str-0x4A4.bin"
  makeCreditsString 4, credCenterX_str_0x4AC, $38, "out/script/credits/str-0x4AC.bin"
  makeCreditsString 4, credCenterX_str_0x4B8, $48, "out/script/credits/str-0x4B8.bin"
  makeCreditsString 4, credCenterX_str_0x4C4, $58, "out/script/credits/str-0x4C4.bin"
  makeCreditsString 4, credCenterX_str_0x4D0, $68, "out/script/credits/str-0x4D0.bin"
  makeCreditsString 4, credCenterX_str_0x4DC, $78, "out/script/credits/str-0x4DC.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x4E9, $38, "out/script/credits/str-0x4E9.bin"
;  makeCreditsString 4, $80, $30, "out/script/credits/str-0x4F7.bin"
  makeCreditsString 4, credCenterX_str_0x506, $50, "out/script/credits/str-0x506.bin"
  makeCreditsString 4, credCenterX_str_0x512, $60, "out/script/credits/str-0x512.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x521, $38, "out/script/credits/str-0x521.bin"
  makeCreditsString 4, credCenterX_str_0x52B, $50, "out/script/credits/str-0x52B.bin"
  makeCreditsString 4, credCenterX_str_0x539, $60, "out/script/credits/str-0x539.bin"
  addCreditsDelay 5

  ; "supervisors"
  makeCreditsString 4, credCenterX_str_0x546, $20, "out/script/credits/str-0x546.bin"
  makeCreditsString 4, credCenterX_str_0x55A, $38, "out/script/credits/str-0x55A.bin"
  makeCreditsString 4, credCenterX_str_0x566, $48, "out/script/credits/str-0x566.bin"
  makeCreditsString 4, credCenterX_str_0x572, $58, "out/script/credits/str-0x572.bin"
  makeCreditsString 4, credCenterX_str_0x57E, $68, "out/script/credits/str-0x57E.bin"
  addCreditsDelay 5

  ; "technical staff"
  makeCreditsString 4, credCenterX_str_0x58B, $20, "out/script/credits/str-0x58B.bin"
  makeCreditsString 4, credCenterX_str_0x5A3, $38, "out/script/credits/str-0x5A3.bin"
  makeCreditsString 4, credCenterX_str_0x5AF, $48, "out/script/credits/str-0x5AF.bin"
  makeCreditsString 4, credCenterX_str_0x5BB, $58, "out/script/credits/str-0x5BB.bin"
  makeCreditsString 4, credCenterX_str_0x5C7, $68, "out/script/credits/str-0x5C7.bin"
  addCreditsDelay 5

  ; "special thanks"
  makeCreditsString 8, credCenterX_str_0x5D4, $20, "out/script/credits/str-0x5D4.bin"
  makeCreditsString 4, credCenterX_str_0x5EC, $38, "out/script/credits/str-0x5EC.bin"
  makeCreditsString 4, credCenterX_str_0x5F8, $48, "out/script/credits/str-0x5F8.bin"
  makeCreditsString 4, credCenterX_str_0x604, $58, "out/script/credits/str-0x604.bin"
  makeCreditsString 4, credCenterX_str_0x610, $68, "out/script/credits/str-0x610.bin"
  addCreditsDelay 4

  makeCreditsString 4, credCenterX_str_0x61D, $38, "out/script/credits/str-0x61D.bin"
  makeCreditsString 4, credCenterX_str_0x62B, $48, "out/script/credits/str-0x62B.bin"
  makeCreditsString 4, credCenterX_str_0x637, $58, "out/script/credits/str-0x637.bin"
  makeCreditsString 4, credCenterX_str_0x643, $68, "out/script/credits/str-0x643.bin"
  makeCreditsString 4, credCenterX_str_0x653, $78, "out/script/credits/str-0x653.bin"
  makeCreditsString 4, credCenterX_str_0x65F, $88, "out/script/credits/str-0x65F.bin"
  addCreditsDelay 5

/*  ; "support"
  makeCreditsString 12, $30-$8, $20, "out/script/credits/str-0x66C.bin"
  ; aoni
  makeCreditsString 4, $60+$8, $20, "out/script/credits/str-0x674.bin"
  ; onkio
  makeCreditsString 4, $60+$8, $38, "out/script/credits/str-0x68A.bin"
  ; cap
  makeCreditsString 4, $60+$8, $50, "out/script/credits/str-0x698.bin"
  addCreditsDelay 4

  ; sun produce
  makeCreditsString 4, $60+$8, $20, "out/script/credits/str-0x6A3.bin"
  ; studio ion
  makeCreditsString 4, $60+$8, $38, "out/script/credits/str-0x6B7.bin"
  ; tic tac
  makeCreditsString 4, $60+$8, $50, "out/script/credits/str-0x6C9.bin"
;  makeCreditsString 4, $70, $60, "out/script/credits/str-0x6DB.bin"
  addCreditsDelay 4

  ; tokyo movie shinsha
  makeCreditsString 4, $60+$8, $20, "out/script/credits/str-0x6EE.bin"
  ; trafico
  makeCreditsString 4, $60+$8, $38, "out/script/credits/str-0x702.bin"
  ; musical plan
  makeCreditsString 4, $60+$8, $50, "out/script/credits/str-0x710.bin"
  ; "in alphabetical order"
  makeCreditsString 4, $90-$18, $68, "out/script/credits/str-0x728.bin"
  addCreditsDelay 5*/

  ; "support"
  makeCreditsString 12, $30-$8, $20, "out/script/credits/str-0x66C.bin"
  ; aoni
  makeCreditsString 4, $60+$8, $20, "out/script/credits/str-0x674.bin"
  ; cap
  makeCreditsString 4, $60+$8, $38, "out/script/credits/str-0x698.bin"
  ; musical plan
  makeCreditsString 4, $60+$8, $50, "out/script/credits/str-0x710.bin"
  addCreditsDelay 4

  ; onkio
  makeCreditsString 4, $60+$8, $20, "out/script/credits/str-0x68A.bin"
  ; studio ion
  makeCreditsString 4, $60+$8, $38, "out/script/credits/str-0x6B7.bin"
  ; sun produce
  makeCreditsString 4, $60+$8, $50, "out/script/credits/str-0x6A3.bin"
  addCreditsDelay 4

  ; tic tac
  makeCreditsString 4, $60+$8, $20, "out/script/credits/str-0x6C9.bin"
;  makeCreditsString 4, $70, $60, "out/script/credits/str-0x6DB.bin"
  ; tokyo movie shinsha
  makeCreditsString 4, $60+$8, $38, "out/script/credits/str-0x6EE.bin"
  ; trafico
  makeCreditsString 4, $60+$8, $50, "out/script/credits/str-0x702.bin"
  ; "in alphabetical order"
  makeCreditsString 4, $90-$18, $68, "out/script/credits/str-0x728.bin"
  addCreditsDelay 5
;  addCreditsDelay (5|$20)

  ; "general director"
  makeCreditsString 8, credCenterX_str_0x739, $30, "out/script/credits/str-0x739.bin"
  makeCreditsString 8, credCenterX_str_0x743, $50, "out/script/credits/str-0x743.bin"
  addCreditsDelay 9
  
  ; terminator
  .db $FF
  
  doCreditsSpecialDelayCheck:
    ; currently, A = raw delay value read from script
/*    pha
      ; check if bit 5 is set = use shortened delay for lag compensation
      and #$20
      beq +
        ; set delayFrameCounter to a higher value,
        ; so the next command happens sooner
        lda #30
        sta $38FB.w
      +:
    pla*/
    
    ; actually, here's a simpler idea: just offset all delays by a tiny amount
    ; that accumulates to what we want.
    ; feasible?
    ; apparently, yes, just barely.
    pha
      ; tick interpolation counter
      ; (we need to reduce each delay by somewhere between 1 and 2 frames
      ; in order to get the desired timing)
      lda @delayCounter.w
      ina
      sta @delayCounter.w
      ; every Nth delay is reduced by 2 frames instead of 1
      cmp #3
      beq +
        ; reduce each individual delay by this many frames.
        ; the goal is to have the gong crash at the end occur at exactly
        ; the same time that the "general director" credit appears.
        lda #$01
        sta $38FB.w
        bra @finish
      +:
        stz @delayCounter.w
        lda #$02
        sta $38FB.w
      @finish:
    pla
    
    ; make up work
    ; set nextDelayRemainingSecs
    sta $38FC.w
    rts
    
    @delayCounter:
      .db $00
.ends

;===================================
; credits lag compensation
;===================================

; translation lags slightly more due to more complicated text compositing
; (probably), so we very slightly reduce delays throughout the sequence
; so that the overall timing remains the same

.bank metabank_intro slot std_slot
.orga $90C7
.section "intro: credits special delay 1" overwrite
  jsr doCreditsSpecialDelayCheck
.ends

;.bank metabank_intro slot std_slot
;.section "intro: credits special delay 2" free
;.ends

;===================================
; "the end"
;===================================

; old sprite state table for "the end" animation
; TODO: are states beyond 0x1D used?
;.unbackground metabank_intro*sizeOfMetabank+$953E metabank_intro*sizeOfMetabank+$9597

; random unused (probably) sprite states
.unbackground metabank_intro*sizeOfMetabank+$95DC metabank_intro*sizeOfMetabank+$9643

.macro sprOp_jump ARGS dst
  .db $04
    .dw dst
.endm

.macro sprOp_show ARGS state, delay, unk1
  .db $0A
    .db state
    .db delay
    .db unk1
.endm

;=====
; load additional graphics
;=====

;.define newTheEndGrp_sectorH $00
;.define newTheEndGrp_sectorM $4B
;.define newTheEndGrp_sectorL $C9
.define newTheEndGrp_sectorH $00
.define newTheEndGrp_sectorM $10
.define newTheEndGrp_sectorL $B4

.define newTheEndGrp_loadAddr $6000
;.define newTheEndGrp_size $6000

.fopen "../out/grp/theend_all_raw.bin" fp
  .fsize fp newTheEndGrp_size
.fclose fp

.define newTheEndGrp_sectorSize (newTheEndGrp_size+$7FF)/$800

.bank metabank_intro slot std_slot
.orga $A292
.section "intro: load new the end graphics 1" overwrite
  doTrampolineCallNew1 loadNewTheEndGraphics
.ends

.bank metabank_new1 slot std_slot
.section "new1: load new the end graphics 2" free
  loadNewTheEndGraphics:
    -:
      ; type = vram read by record count
      lda #$FF
      sta _DH.b
      ; load addr
      lda #<newTheEndGrp_loadAddr
      sta _BL.b
      lda #>newTheEndGrp_loadAddr
      sta _BH.b
      ; length
      lda #<newTheEndGrp_sectorSize
      sta _AL.b
;      lda #>newTheEndGrp_size
;      sta _AH.b
      ; src sector
      lda #newTheEndGrp_sectorH
      sta _CL
      lda #newTheEndGrp_sectorM
      sta _CH
      lda #newTheEndGrp_sectorL
      sta _DL
      ; load
      jsr CD_READ
      ; loop until read successful
      and #$FF
      bne -
    
    ; make up work
    lda #$47
    sta $2D.b
    lda #$B2
    rts
.ends

;=====
; new sprite states
;=====

; oh wait, we can't do this because the fireworks sequence also uses states
; based out of this same table.
; good thing the programmers left lots of blank states just in case they
; needed more!

/*.bank metabank_kernel slot std_slot
.orga $4457
.section "kernel: new the end sprite states 1" overwrite
  ; pointer to state id -> sprite list table for "the end" object
;  .dw $953E
  .dw theEndAnim_spriteList
.ends

.bank metabank_intro slot std_slot
.section "intro: new the end sprite states 2" free
  theEndAnim_spriteList:
    
.ends*/

.bank metabank_intro slot std_slot
.orga $959A
.section "intro: new the end sprite states 1" overwrite
  ; this overwrites unused blank entries starting from state 0x2E
  
  ; state 0x2E
  .dw extraTheEndSpriteState_0
  ; state 0x2F
  .dw extraTheEndSpriteState_1
  ; state 0x30
  .dw extraTheEndSpriteState_2
  ; state 0x31
  .dw extraTheEndSpriteState_3
  ; state 0x32
  .dw extraTheEndSpriteState_4
  ; state 0x33
  .dw extraTheEndSpriteState_5
  ; state 0x34
  .dw extraTheEndSpriteState_6
  ; state 0x35
  .dw extraTheEndSpriteState_7
.ends

.bank metabank_intro slot std_slot
.section "intro: new the end sprite states 2" free
;  extraTheEndSpriteState_1:
;    ; sprite count
;    .db $04
;    .db $11,$10,$10,$E0,$80,$02
  
  extraTheEndSpriteState_0:
    .incbin "out/grp/theend_fade0_sprites_raw.bin"
  extraTheEndSpriteState_1:
    .incbin "out/grp/theend_fade1_sprites_raw.bin"
  extraTheEndSpriteState_2:
    .incbin "out/grp/theend_fade2_sprites_raw.bin"
  extraTheEndSpriteState_3:
    .incbin "out/grp/theend_fade3_sprites_raw.bin"
  extraTheEndSpriteState_4:
    .incbin "out/grp/theend_fade4_sprites_raw.bin"
  extraTheEndSpriteState_5:
    .incbin "out/grp/theend_fade5_sprites_raw.bin"
  extraTheEndSpriteState_6:
    .incbin "out/grp/theend_fade6_sprites_raw.bin"
  extraTheEndSpriteState_7:
    .incbin "out/grp/theend_fade7_sprites_raw.bin"
.ends

;=====
; redirect animation script
;=====

.define theEndFadeSpeed 3
.define theEndPreDelay 90

.bank metabank_intro slot std_slot
.orga $BC0F
.section "intro: new the end animation script 1" overwrite
  sprOp_jump extraTheEndAnimScripting
.ends

.bank metabank_intro slot std_slot
.orga $BC13
.section "intro: new the end animation script 2" overwrite
  ; show this while waiting for player to press button
;  sprOp_show $2D, $0A, $83
  sprOp_show $2E, $0A, $80
.ends

.bank metabank_intro slot std_slot
.section "intro: new the end animation script 3" free
  extraTheEndAnimScripting:
    ; make up work
;    sprOp_show $2D, $70, $83
;    sprOp_show $2D, $0A, $83
    
    ; pause on complete graphic
;    sprOp_show $2D, $38, $83
    sprOp_show $2D, theEndPreDelay, $83
    
    sprOp_show $35, theEndFadeSpeed, $80
    sprOp_show $34, theEndFadeSpeed, $80
    sprOp_show $33, theEndFadeSpeed, $80
    sprOp_show $32, theEndFadeSpeed, $80
    sprOp_show $31, theEndFadeSpeed, $80
    sprOp_show $30, theEndFadeSpeed, $80
    sprOp_show $2F, theEndFadeSpeed, $80
    
    ; pause after fade completes
;    sprOp_show $2E, $70, $80
    sprOp_show $2E, $38, $80
;    sprOp_show $2E, $0A, $80
    
    
    sprOp_jump $BC17
.ends

;==============================================================================
; special
;==============================================================================

;.unbackground metabank_special*sizeOfMetabank+$A9D0 metabank_special*sizeOfMetabank+$BFFF
; $A000-AFFF are reserved for the special message shown before the save prompt
; when you're sent to gokumon
; (this is way more space than it actually needs, but then we don't need the space
; for anything else, either)
.unbackground metabank_special*sizeOfMetabank+$B000 metabank_special*sizeOfMetabank+$BFFF

;===================================
; theater credits
;===================================

.bank metabank_special slot std_slot
.orga $933F
.section "special: new theater credits sprite state 1" overwrite
  .dw newTheaterCreditsSpriteState
.ends

.bank metabank_special slot std_slot
.section "special: new theater credits sprite state 2" free
  newTheaterCreditsSpriteState:
    .incbin "out/grp/theater_credits_sprite.bin"
.ends

;===================================
; adjust x-pos of theater credits
;===================================

.bank metabank_special slot std_slot
.orga $A85E
.section "special: theater credits base x-offset 1" overwrite
  ; note: signed value
;  .db $C0
  .db $A0
.ends

;===================================
; theater end
;===================================

.define numNewTheaterEndSprites 2

.bank metabank_special slot std_slot
.orga $932B
.section "special: new theater end sprite states 1" overwrite
  .dw newTheaterEndSpriteState_0
  .dw newTheaterEndSpriteState_1
  .dw newTheaterEndSpriteState_2
.ends

.bank metabank_special slot std_slot
.section "special: new theater end sprite states 2" free
  newTheaterEndSpriteState_0:
    ; sprite count
    .db $05+numNewTheaterEndSprites
    ; original data
    .db $00,$70,$E0,$E0,$80,$01
    .db $10,$70,$00,$E0,$80,$01
    .db $26,$00,$C0,$C0,$80,$01
    .db $20,$00,$30,$E0,$80,$01
    .db $37,$00,$A0,$F0,$80,$01
    ; new data
    .incbin "out/grp/theater_end_sprite.bin" SKIP 1
    
  newTheaterEndSpriteState_1:
    ; sprite count
    .db $05+numNewTheaterEndSprites
    ; original data
    .db $00,$70,$E0,$E0,$80,$01
    .db $10,$70,$00,$E0,$80,$01
    .db $37,$00,$C0,$C0,$80,$01
    .db $26,$00,$30,$E0,$80,$01
    .db $20,$00,$A0,$F0,$80,$01
    ; new data
    .incbin "out/grp/theater_end_sprite.bin" SKIP 1
    
  newTheaterEndSpriteState_2:
    ; sprite count
    .db $05+numNewTheaterEndSprites
    ; original data
    .db $00,$70,$E0,$E0,$80,$01
    .db $10,$70,$00,$E0,$80,$01
    .db $20,$00,$C0,$C0,$80,$01
    .db $37,$00,$30,$E0,$80,$01
    .db $26,$00,$A0,$F0,$80,$01
    ; new data
    .incbin "out/grp/theater_end_sprite.bin" SKIP 1
.ends

;===================================
; theater act titles
;===================================

.bank metabank_special slot std_slot
.orga $9341
.section "special: new theater act title sprite states 1" overwrite
  .dw newTheaterActTitleSpriteState
.ends

.bank metabank_special slot std_slot
.section "special: new theater act title sprite states 2" free
  ; all acts are displayed using the same sprite state but load different
  ; graphics to change their appearance
  newTheaterActTitleSpriteState:
    .incbin "out/grp/theater_acts_1_sprite.bin"
.ends

;==============================================================================
; new4 (non-scd error message)
;==============================================================================

;===================================
; unbackgrounds
;===================================

.unbackground metabank_new4*sizeOfMetabank+newArea4LoadAddr+$0000 metabank_new4*sizeOfMetabank+newArea4LoadAddr+$0FFF

;===================================
; 
;===================================

;.define paletteRawAddr $2BD6

.bank metabank_new4 slot std_slot
.orga $C000
.section "new4: scd error message 1" force
  scdErrorMsgStart:
    ; load vram for error message
    -:
      ; REC H
      lda #scdErrorVramSrcSectorH
      sta _CL.b
      ; REC M
      lda #scdErrorVramSrcSectorM
      sta _CH.b
      ; REC L
      lda #scdErrorVramSrcSectorL
      sta _DL.b
      ; type = vram by sector
      lda #$FF
      sta _DH.b
      ; dst
      lda #<scdErrorVramLoadAddr
      sta _BL.b
      lda #>scdErrorVramLoadAddr
      sta _BH.b
      ; length
      lda #scdErrorVramSectorCount
      sta _AL.b
      
      jsr CD_READ
      and #$FF
      bne -
    
    ; wait until end of frame so we don't upload our palette during
    ; active display
    jsr doStdFrameEnd
    
    ; load palette for error message
    ; target palette 0, color 0
    lda #$00
    sta vce_ctaLo.w
    ; target bg palettes
    lda #$00
    sta vce_ctaHi.w
    ; copy colors to vce
    tia scdErrorMsgPalette,vce_ctwLo,(scdErrorMsgPalette_end-scdErrorMsgPalette)
    
    ; wait until run button pressed
    -:
      jsr doStdFrameEnd
      
      lda JOY.w
      and #$08
      beq -
    
    ; set cd base track to 06 (original game's backup track) using CD_BASE
    ; mode = by track
    lda #$80
    sta _BH.b
    ; set = both base and backup
    lda #$00
    sta _CL.b
    ; track = 6 (bcd)
    lda #$06
    sta _AL.b
    jsr CD_BASE
    
    ; reload original kernel from backup track using same specifications as ipl
    -:
      ; REC H
      lda #$00
      sta _CL.b
      ; REC M
      lda #$00
      sta _CH.b
      ; REC L
      lda #$09
      sta _DL.b
      ; type = read through MPR6
      lda #6
      sta _DH.b
      ; dst bank num
      lda #$80
      sta _BL.b
      ; length
      lda #$0A
      sta _AL.b
      
      jsr CD_READ
      and #$FF
      bne -
    
    ; hope for the best
    jmp $4000
  
  scdErrorMsgPalette:
    .incbin "rsrc_raw/pal/scderror.pal"
  scdErrorMsgPalette_end:
.ends

