
;==============================================================================
; required defines
;==============================================================================

;==============================================================================
; various overwrites and patches
;==============================================================================

;=============================
; sync var
;=============================

; TODO
/*.bank fixedBank slot fixedSlot
.section "var sync 1" free
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
.ends*/

;=============================
; adpcm playback sync
;=============================



;==============================================================================
; the main event!
;==============================================================================

;=============================
; 
;=============================

/*.bank fixedBank slot 0
.orga syncVector+($5284-$527F)
.section "sync handler injection 1" overwrite
  ; ensure the banks containing our code are loaded
;  jsr ovlScene_jumpTable_setUpStdBanks
  jsr ovlScene_setUpStdBanks
  ; run the new code
  jmp newSyncLogic
.ends*/

;.block "new stuff"
.bank freeBank slot freeSlot
.section "new sync handler logic 2" free
  newScriptResetAreaStart:
    ; frame counter, incremented every vsync.
    ; used to time script events
    syncFrameCounter:
      .dw $00
    ; DEBUG: same as above, but never gets reset.
    ; used to check timing
    globalSyncFrameCounter:
      .dw $00
    syncVar:
      .db $00
    syncFrameCounterAtLastVarSync:
      .dw $0000
    blockVramWrites:
      .db $00
    lastSyncId:
      .db $00
  newScriptResetAreaEnd:
  
;  oldPaletteTransferFlag:
;    .db $00
  
  ; these are written every frame,
  ; starting at color index 1 of the target palette.
  ; colors 1 and 3 are the font color,
  ; color 2 is the drop shadow color
  paletteOverrideColors:
    ; if bit 1 is set, color is font.
    ; otherwise, it's shadow
    .dw defaultSubColor
    .rept 7
      .dw defaultSubShadowColor
      .dw defaultSubColor
    .endr
  paletteOverrideColorsEnd:
  
  resetForScriptStart:
    tai blockClearWord,newScriptResetAreaStart,(newScriptResetAreaEnd-newScriptResetAreaStart)
    stz subtitleDisplayOn.w
    rts
  
  newSyncLogic:
    ; do nothing if subtitle engine not on
    lda subtitleEngineOn.w
    bne +
      jmp @done
    +:
    
    ; increment frame counter
    inc syncFrameCounter+0.w
    bne +
      inc syncFrameCounter+1.w
    +:
    ; DEBUG
    inc globalSyncFrameCounter+0.w
    bne +
      inc globalSyncFrameCounter+1.w
    +:
    
    lda nextRcrCropTargetsMultipleLines.w
    sta rcrCropTargetsMultipleLines.w
    
    lda subtitleDisplayOn.w
    sta prevSubtitleDisplayOn.w
    
    ; check if asynchronous subtitle clear has occurred
    lda queuedSubsOffIsOn.w
    beq +
      lda queuedSubsOffTime+0.w
      sec
      sbc syncFrameCounter.w
      lda queuedSubsOffTime+1.w
      sbc syncFrameCounter+1.w
      bcs ++
        ; turn subtitles off
        jsr turnSubsOff
        stz queuedSubsOffIsOn.w
      ++:
    +:
    
    ; display subs if on
    lda subtitleDisplayOn.w
    beq +
/*      ;=====
      ; write subtitle sprite attributes to start of SAT
      ;=====
      
      ; if auto vreg protect is on, assume we cannot touch mawr
      ; if flag is set, and do not attempt to force sprites
      ; (afaict the only time this ever happens is the credits,
      ; because text printing does not protect its mawr sets)
      .ifdef enableSceneAutoVregProtect
        bbr6 $F5,+
      .endif
      
      ; set write address
      st0 #$00
      st1 #<satVramAddr
      st2 #>satVramAddr
      
      ; start write
      st0 #$02
      
      lda currentSubtitleSpriteAttributeQueuePtr+0.w
      sta @transferToSatCmd+1.w
      lda currentSubtitleSpriteAttributeQueuePtr+1.w
      sta @transferToSatCmd+2.w
      
      lda currentSubtitleSpriteAttributeQueueSize.w
      sta @transferToSatCmd+5.w
      
      ; TODO: possible optimization: write the queue pointer here directly
      ; if it's never needed elsewhere
      @transferToSatCmd:
      tia $0000,$0002,$0000
      
      ; TODO: is this necessary?
      ; the real problem with flickering, etc. may simply be
      ; the sprites getting turned off (and then immediately on
      ; again by us)
      ; if non-subtitle sprite table clear requested, handle that
      lda triggerNonSubtitleSpriteClear.w
      beq ++
        ; blank everything EXCEPT the subtitle sprites from the SAT.
        ; the address is already primed (we just wrote all the
        ; subtitle sprites, and we're clearing out everything else
        ; to the end of the SAT)
        
        ; set size of the area to blank
        lda currentSubtitleSpriteAttributeQueueSize.w
        lsr
        eor #$FF
        ina
        tax
        jsr doVramClear
        
        ; clear trigger flag
        stz triggerNonSubtitleSpriteClear.w
      ++:
      
      ; initiate sat->satb dma
      st0 #$13
      st1 #<satVramAddr
      st2 #>satVramAddr
      
      ; force sprite display on
      smb6 $F3
;      lda $F3
;      ora #$40
;      st0 #$05
;      sta $0002.w*/
    
      ;=====
      ; override palette for subtitles
      ;=====
      
      ; set override palette
      ; dstaddr
  ;    lda #$81
      lda currentSubtitlePaletteIndex.w
      ; bit 7 of index set = disabled
      bmi ++
        ; could optimize this by precomputing the shift,
        ; but i don't think it'll come to that
        asl
        asl
        asl
        asl
        ; target from color 1
        ora #$01
        sta vce_ctaLo.w
        ; target sprite palettes
        lda #$01
        sta vce_ctaHi.w
        ; copy colors to vce
        tia paletteOverrideColors,vce_ctwLo,(paletteOverrideColorsEnd-paletteOverrideColors)
      ++:
    +:
    
    lda newZpFreeReg
    pha
    lda newZpFreeReg+1
    pha
    lda newZpScriptReg
    pha
    lda newZpScriptReg+1
    pha
        
/*      .ifdef useLagAutoProtection
        lda lagAutoProtectCounterAddrB.b
        cmp #$02
        bcs @actionsDone
      .endif
        
      .ifdef useFadeAutoProtection
        lda fadeOn.w
        beq @noFadeProtect
          cly
          clx
          @byteCountLoop:
            lda fadeBgBase.w,X
            phx
              clx
              @bitCountLoop:
                asl
                bcc @notSet
                  iny
                @notSet:
                inx
                cpx #$08
                bne @bitCountLoop
            plx
            inx
            cpx #$04
            bne @byteCountLoop
          
          @fadeCheckDone:
          tya
          cmp #$04
          bcs @actionsDone
          
        @noFadeProtect:
      .endif
      
      .ifdef useBigPaletteWriteProtection
        lda bigPaletteWriteOccurredB.b
        bne @actionsDone
      .endif*/
      
      stz @didGrpTransfer.w
      
      lda spriteGrpGenAndTransferNeeded.w
      beq +
        ; cannot touch mawr if autoprotect on and flagged
;        .ifdef enableSceneAutoVregProtect
;          bbr6 $F5,@actionsDone
;        .endif
        lda blockVramWrites.w
        bne @actionsDone
      
        jsr doSpriteGrpGenAndTransfer
        inc @didGrpTransfer.w
        ; allow attribute generation after a graphics transfer
        ; TODO: safe?
        ; also, in practice, i'm pretty sure attribute generation
        ; will never be triggered without a preceding graphics
        ; transfer, so this could probably be simplified somehow
;          bra @actionsDone
      +:
      
      lda spriteAttrGenAndTransferNeeded.w
      beq +
        ; cannot touch mawr if autoprotect on and flagged
;        .ifdef enableSceneAutoVregProtect
;          bbr6 $F5,@actionsDone
;        .endif
        
        jsr doSpriteAttrGenAndTransfer
        bra @actionsDone
      +:
      
      ; do not evaluate script if we did a graphics transfer
      lda @didGrpTransfer.w
      bne @actionsDone
      
      ;=====
      ; run script
      ;=====
      
      lda subtitleScriptPtr.w
      sta newZpScriptReg
      lda subtitleScriptPtr+1.w
      sta newZpScriptReg+1
      
;        lda #maxScriptActionsPerIteration
;        sta remainingScriptActions.w
;        -:
        jsr runScript
;          dec remainingScriptActions.w
;          bne -
      
      lda newZpScriptReg
      sta subtitleScriptPtr.w
      lda newZpScriptReg+1
      sta subtitleScriptPtr+1.w
      
    @actionsDone:
    pla
    sta newZpScriptReg+1
    pla
    sta newZpScriptReg
    pla
    sta newZpFreeReg+1
    pla
    sta newZpFreeReg
    @done:
    ; restore old banks
;    jmp ovlScene_jumpTable_restoreOldBanks
;    jmp ovlScene_restoreOldBanks
    rts
    
    @didGrpTransfer:
      .db $00
    
;  remainingScriptActions:
;    .db $00
  
  runScript:
    cly
    
    lda (newZpScriptReg),Y
    cmp #sceneFontCharsBase
    bcc @isOpcode
    
    @isLiteral:
      jsr printSubtitleChar
      lda #$01
      jmp addToScriptPtr
    @isOpcode:
      cmp #sceneOp_terminator
      beq @done
    
      @doGenericSceneOp:
      asl
      tax
      jmp (subtitleOpJumpTable,X)
    
    @done:
    ; when terminator reached, shut off subtitle engine
    stz subtitleEngineOn.w
    rts
  
  subtitleOpJumpTable:
    ; 00 = terminator (special-cased)
    .dw $0000
    ; 01 = sceneOp_waitForFrame
    .dw sceneOp_waitForFrame_handler
    ; 02 = sceneOp_br
    .dw sceneOp_br_handler
    ; 03 = sceneOp_resetCompBuffers
;    .dw sceneOp_resetCompBuffers_handler
    .dw sceneOp_subsOffNoClear_handler
    ; 04
;    .dw sceneOp_prepAndSendGrp_handler
    .dw sceneOp_subsOnNoClear_handler
    ; 05
    .dw sceneOp_swapAndShowBuf_handler
    ; 06
    .dw sceneOp_subsOff_handler
    ; 07
    .dw sceneOp_finishCurrentLine_handler
    ; 08
    .dw sceneOp_setPalette_handler
    ; 09
    .dw sceneOp_writeMem_handler
    ; 0A
    .dw sceneOp_setCropOn_handler
    ; 0B
;    .dw sceneOp_resetSyncTimer_handler
;    .dw sceneOp_subtractFromSyncTimer_handler
    .dw sceneOp_startNewString_handler
    ; 0C
;    .dw sceneOp_writePalette_handler
    .dw sceneOp_waitForSyncVar_handler
    ; 0D
;    .dw sceneOp_writeVram_handler
    .dw sceneOp_writePalette_handler
;    .dw $0000
    ; 0E
;    .dw sceneOp_waitForAdpcm_handler
    .dw sceneOp_jump_handler
    ; 0F
    .dw sceneOp_queueSubsOff_hander
    ; 10
    .dw sceneOp_jumpIfMaskedEq_hander
    ; 11
    .dw sceneOp_writeMemWord_handler
    ; 12
    .dw sceneOp_setBoxFadeLevel_handler
    ; 13
    .dw sceneOp_setLineFloodMode_handler
    ; 14
    .dw sceneOp_jumpIfLastSyncIdEq_handler
    ; 15
    .dw sceneOp_swapBuf_handler
  
  sceneOp_waitForFrame_handler:
    iny
    lda (newZpScriptReg),Y
    sec
    sbc syncFrameCounter.w
    iny
    lda (newZpScriptReg),Y
    sbc syncFrameCounter+1.w
    bcs @endCurrentIteration
      tya
      ina
      jmp addToScriptPtr
    @endCurrentIteration:
;    lda #$01
;    sta remainingScriptActions.w
    rts
  
  sceneOp_br_handler:
    ; if we have a pending (nonempty) sprite, send it first
    lda @finalSendTriggered.w
    bne @done
      ; if line is empty, skip normal send
      lda activeSubtitleXPos.w
      beq @done
      
      ; increment nonempty line count for corresponding group
      lda activeSubtitleLineNum.w
      lsr
      tax
      inc groupNonemptyLineCountArray.w,X
    
      ; as a special case: if the final x-size of the line is divisible
      ; by 16, we need to add one extra, empty sprite for the rightmost
      ; column of the outline to go in
      lda activeSubtitleXPos.w
      and #$0F
      bne ++
        ; since the x-size is at a sprite boundary,
        ; the comp buffer is guaranteed to be empty at this point,
        ; so we can just send it again.
        ; the leftover contents of the drop shadow buffer will be
        ; added in when the (empty) graphic is transferred.
        ; we are essentially pretending we printed a one-pixel space
        ; and now need to send it
        inc activeSubtitleXPos.w
      ++:
      
      ; do not advance the script pointer; we will return here
      ; once the transfer completes
      jsr sendNextSpriteBufferAndForceAttributeSend
      inc @finalSendTriggered.w
;      lda #$01
;      sta remainingScriptActions.w
      rts
    +:
    
    @done:
    
    stz @finalSendTriggered.w
    
    ; finish the line
    jsr endCurrentLineComp
    
    lda #$01
    jmp addToScriptPtr
    
    @finalSendTriggered:
    .db $00
  
  sceneOp_subsOffNoClear_handler:
    stz subtitleDisplayOn.w
    
    lda #$01
    jmp addToScriptPtr
  
  sceneOp_subsOnNoClear_handler:
    lda #$FF
    sta subtitleDisplayOn.w
    
    lda #$01
    jmp addToScriptPtr
  
  sceneOp_swapAndShowBuf_handler:
    jsr endCurrentSubComp
    lda #$FF
    @finish:
    sta subtitleDisplayOn.w
    
    ; reset composition buffers for next string
;    jsr resetSubtitleCompBuffers
    
    lda #$01
    jmp addToScriptPtr
  
  sceneOp_swapBuf_handler:
    jsr endCurrentSubComp
    cla
    bra sceneOp_swapAndShowBuf_handler@finish
  
  sceneOp_subsOff_handler:
    jsr turnSubsOff
    
    ; prevent further evaluation of script
;    lda #$01
;    sta remainingScriptActions.w
    
    lda #$01
    jmp addToScriptPtr
  
  sceneOp_finishCurrentLine_handler:
;    jsr finishCurrentSubtitleBufferLine
    ; TODO: good enough?
    jmp sceneOp_br_handler
;    lda #$01
;    jmp addToScriptPtr
  
  sceneOp_setPalette_handler:
    iny
    lda (newZpScriptReg),Y
    sta currentSubtitlePaletteIndex.w
    
    lda #$02
    jmp addToScriptPtr
  
  sceneOp_writeMem_handler:
    ; FIXME: we're technically allowing script data to be in the bank
    ; that we're loading in here; a safer solution would be to
    ; copy the parameters before processing them.
    ; but i don't expect it to be an issue this time around,
    ; so i'm going with this for now.
    tma #$40
    pha
      ; set up bank
      iny
      lda (newZpScriptReg),Y
      tam #$40
      
      ; set up address
      iny
      lda (newZpScriptReg),Y
      sta @writeInstr+1.w
      iny
      lda (newZpScriptReg),Y
      sta @writeInstr+2.w
      
      ; load and write value
      iny
      lda (newZpScriptReg),Y
      @writeInstr:
      sta $0000.w
    pla
    tam #$40
    
    lda #$05
    jmp addToScriptPtr
  
  sceneOp_writeMemWord_handler:
    ; FIXME: we're technically allowing script data to be in the bank
    ; that we're loading in here; a safer solution would be to
    ; copy the parameters before processing them.
    ; but i don't expect it to be an issue this time around,
    ; so i'm going with this for now.
    tma #$40
    pha
      ; set up bank
      iny
      lda (newZpScriptReg),Y
      tam #$40
      
      ; set up address
      iny
      lda (newZpScriptReg),Y
      sta @writeInstr+1.w
      iny
      lda (newZpScriptReg),Y
      sta @writeInstr+2.w
      
      ; load and write value
      clx
      -:
        iny
        lda (newZpScriptReg),Y
        @writeInstr:
        sta $0000.w,X
        
        inx
        cpx #2
        bne -
    pla
    tam #$40
    
    lda #$06
    jmp addToScriptPtr
  
  sceneOp_setBoxFadeLevel_handler:
    lda #$02
    jmp addToScriptPtr
  
  sceneOp_setLineFloodMode_handler:
    iny
    lda (newZpScriptReg),Y
    sta lineFloodModeOn.w
    lda #$02
    jmp addToScriptPtr
  
  sceneOp_jumpIfLastSyncIdEq_handler:
    iny
    lda (newZpScriptReg),Y
    cmp lastSyncId.w
    bne @done
      jmp sceneOp_jump_handler
    @done:
    lda #$04
    jmp addToScriptPtr
    
  
  sceneOp_setCropOn_handler:
;    iny
;    lda (newZpScriptReg),Y
;    sta newRcrOn.w
    
    lda #$02
    jmp addToScriptPtr
  
/*  sceneOp_setHighPrioritySprObjOffset_handler:
    iny
    lda (newZpScriptReg),Y
    sta highPrioritySpriteObjGenerationOffset.w
    
    lda #$02
    jmp addToScriptPtr
    
  sceneOp_setLowPrioritySprObjOffset_handler:
    iny
    lda (newZpScriptReg),Y
    sta lowPrioritySpriteObjGenerationOffset.w
    
    lda #$02
    jmp addToScriptPtr*/
  
  sceneOp_startNewString_handler:
    jsr resetStateForNewString
  
    iny
    lda (newZpScriptReg),Y
    sta nextSpriteAttrVramBase+0.w
    iny
    lda (newZpScriptReg),Y
    sta nextSpriteAttrVramBase+1.w
    
    lda #$03
    jmp addToScriptPtr
  
  sceneOp_waitForSyncVar_handler:
    iny
    lda (newZpScriptReg),Y
    cmp syncVar.w
    beq @done
    bcs @notDone
    @done:
      ; subtract actual trigger time of last adpcm event
      ; from current time.
      ; this accounts for any possible time overrun
      ; (e.g. if a line was still being rendered at the time
      ; the waitForAdpcm event was supposed to occur, causing
      ; it to trigger late)
      lda syncFrameCounter+0.w
      sec
      sbc syncFrameCounterAtLastVarSync+0.w
      sta syncFrameCounter+0.w
      
      lda syncFrameCounter+1.w
      sbc syncFrameCounterAtLastVarSync+1.w
      sta syncFrameCounter+1.w
      
      ; prevent further evaluation of script
      ; (to ensure that the next script action begins at
      ; a frame boundary, for better synchronization)
;      lda #$01
;      sta remainingScriptActions.w
      
      tya
      ina
      jmp addToScriptPtr
    @notDone:
    ; prevent further evaluation of script
;    lda #$01
;    sta remainingScriptActions.w
    rts
    
  sceneOp_writePalette_handler:
    ; src
    lda newZpScriptReg+0
    clc
    adc #$05
    sta @transferCmd.w+1
    cla
    adc newZpScriptReg+1
    sta @transferCmd.w+2
    
    ; size
    iny
    lda (newZpScriptReg),Y
    sta @transferCmd+5.w
    iny
    lda (newZpScriptReg),Y
    sta @transferCmd+6.w
    
    ; dst
    iny
    lda (newZpScriptReg),Y
;    sta @addrCmdLo+1.w
    sta vce_ctaLo.w
    iny
    lda (newZpScriptReg),Y
;    sta @addrCmdHi+1.w
    sta vce_ctaHi.w
    
      ; copy colors to vce
      @transferCmd:
      tia $0000,vce_ctwLo,$0000
    
    ; add size to script ptr
    lda newZpScriptReg+0
    clc
    adc @transferCmd+5.w
    sta newZpScriptReg+0
    lda newZpScriptReg+1
    adc @transferCmd+6.w
    sta newZpScriptReg+1
    
    ; prevent further evaluation of script
;    lda #$01
;    sta remainingScriptActions.w
    
    ; add base size to script ptr
    lda #$05
    jmp addToScriptPtr
  
/*  ; note: it is the caller's responsibility
  ; not to transfer too much at a time
  sceneOp_writeVram_handler:
    ; srcBank
    iny
    lda (newZpScriptReg),Y
    sta @bankLoadCmd+1.w
    
    ; src
    iny
    lda (newZpScriptReg),Y
    clc
    adc #<sceneOp_writeVram_pointerBase
    sta @transferCmd+1.w
    iny
    lda (newZpScriptReg),Y
    adc #>sceneOp_writeVram_pointerBase
    sta @transferCmd+2.w
    
    ; size
    iny
    lda (newZpScriptReg),Y
    sta @transferCmd+5.w
    iny
    lda (newZpScriptReg),Y
    sta @transferCmd+6.w
    
    ; dst
    iny
    lda (newZpScriptReg),Y
    sta @addrCmdLo+1.w
    iny
    lda (newZpScriptReg),Y
    sta @addrCmdHi+1.w
    
    tma #$20
    pha
    tma #$40
    pha
    @bankLoadCmd:
    lda #$00
    tam #$20
    ina
    tam #$40
      st0 #$00
      @addrCmdLo:
      st1 #$00
      @addrCmdHi:
      st2 #$00
      st0 #$02
      
      @transferCmd:
      tia $0000,vdp_data,$0000
    pla
    tam #$20
    pla
    tam #$40
    
    ; prevent further evaluation of script
;    lda #$01
;    sta remainingScriptActions.w
    
    ; add base size to script ptr
    lda #$08
    jmp addToScriptPtr*/
  
/*  sceneOp_waitForAdpcm_handler:
    iny
    lda (newZpScriptReg),Y
    cmp adpcmSyncCounter.w
    beq @done
    bcs @notDone
    @done:
      ; subtract actual trigger time of last adpcm event
      ; from current time.
      ; this accounts for any possible time overrun
      ; (e.g. if a line was still being rendered at the time
      ; the waitForAdpcm event was supposed to occur, causing
      ; it to trigger late)
      lda syncFrameCounter+0.w
      sec
      sbc syncFrameCounterAtLastAdpcmSync+0.w
      sta syncFrameCounter+0.w
      
      lda syncFrameCounter+1.w
      sbc syncFrameCounterAtLastAdpcmSync+1.w
      sta syncFrameCounter+1.w
      
      ; prevent further evaluation of script
      ; (to ensure that the next script action begins at
      ; a frame boundary, for better synchronization)
      lda #$01
      sta remainingScriptActions.w
      
      tya
      ina
      jmp addToScriptPtr
    @notDone:
    ; prevent further evaluation of script
    lda #$01
    sta remainingScriptActions.w
    rts */
  
  sceneOp_jump_handler:
    iny
    lda (newZpScriptReg),Y
    pha
      iny
      lda (newZpScriptReg),Y
      sta newZpScriptReg+1
    pla
    sta newZpScriptReg+0
    rts
  
  sceneOp_queueSubsOff_hander:
    iny
    lda (newZpScriptReg),Y
    sta queuedSubsOffTime+0.w
    iny
    lda (newZpScriptReg),Y
    sta queuedSubsOffTime+1.w
    
    tya
    ina
    sta queuedSubsOffIsOn.w
    jmp addToScriptPtr
  
/*  sceneOp_terminateIfMasked_hander:
    iny
    lda (newZpScriptReg),Y
    sta newZpFreeReg+0
    iny
    lda (newZpScriptReg),Y
    sta newZpFreeReg+1
    
    iny
    lda (newZpFreeReg)
    and (newZpScriptReg),Y
    beq +
      ; if target bit(s) set, terminate script
      jmp runScript@done
    +:
    
    tya
    ina
    jmp addToScriptPtr*/
  
  sceneOp_jumpIfMaskedEq_hander:
    iny
    lda (newZpScriptReg),Y
    sta newZpFreeReg+0
    iny
    lda (newZpScriptReg),Y
    sta newZpFreeReg+1
    
    iny
    lda (newZpFreeReg)
    and (newZpScriptReg),Y
    iny
    cmp (newZpScriptReg),Y
    bne +
      ; if target bit(s) set
;      jmp runScript@done
      jmp sceneOp_jump_handler
    +:
    
    lda #7
    jmp addToScriptPtr
  
  addToScriptPtr:
    clc
    adc newZpScriptReg
    sta newZpScriptReg
    cla
    adc newZpScriptReg+1
    sta newZpScriptReg+1
    rts
  
  ; A = raw codepoint
  printSubtitleChar:
    ; clear char comp buffer
;    jsr clearSubtitleCharCompBuffer
    tai blockClearWord,subtitleCharCompBuffer,(subtitleCharCompBufferEnd-subtitleCharCompBuffer)
    
    ; convert from raw codepoint to font index
    sec
    sbc #sceneFontCharsBase
    
    pha
      ; look up pointer to target char
;      asl
;      tax
;      lda ovlScene_fontLut+0.w,X
;      sta newZpFreeReg+0
;      lda ovlScene_fontLut+1.w,X
;      sta newZpFreeReg+1
      
      ; multiply by 2, saving result separately for future use
      asl
      sta @fontFinalAdd1+1.w
      ; the input will not be greater than 0x80, so the high byte
      ; from multiplying by 2 will always be zero; we don't need to
      ; bother explicitly computing it here
      stz newZpFreeReg+1.b
      
      ; shift left twice to get (raw * 8)
      asl
      rol newZpFreeReg+1.b
      asl
      rol newZpFreeReg+1.b
      
      ; add base pointer for font data
      clc
      adc #<ovlScene_font
      sta newZpFreeReg+0.b
      lda newZpFreeReg+1.b
      adc #>ovlScene_font
      sta newZpFreeReg+1.b
      
      ; add (raw * 2) to (raw * 8) to get (raw * 10)
      @fontFinalAdd1:
      lda #$00
      clc
      adc newZpFreeReg+0.b
      sta newZpFreeReg+0.b
      
  ;    @fontFinalAdd2:
      cla
      adc newZpFreeReg+1.b
      sta newZpFreeReg+1.b
      
      ; copy bitmap to buffer
      ; x starts at (targetLine * 3),
      ; because we want a blank line at the top
      ; to allow for potential outline effects there
      clx
      ; bitshift optimization
      lda activeSubtitleXPos.w
      pha
        and #$0F
        cmp #$05
        bcc +
          inx
        +:
        cmp #$0D
        bcc +
          inx
        +:
        cly
        -:
          lda (newZpFreeReg),Y
          sta subtitleCharCompBuffer.w,X
          iny
          inx
          inx
          inx
          cpx #(bytesPerSceneFontChar*3)
          bcc -
      pla
      
      ; NOTE: original, mostly-unoptimized routine
      and #$07
      beq @noRightShift
      cmp #$05
      bcc @doRightShift
      @doLeftShift:
        eor #$FF
        ina
        and #$03
        ; shift only the lines that are not empty
        clx
        --:
        tay
          -:
            asl subtitleCharCompBuffer+2.w,X
            rol subtitleCharCompBuffer+1.w,X
            rol subtitleCharCompBuffer+0.w,X
            
            dey
            bne -
          inx
          inx
          inx
          cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
          bcc --
        bra @noRightShift
      @doRightShift:
;        clx
        ; shift only the lines that are not empty
        clx
        --:
        tay
          -:
            lsr subtitleCharCompBuffer+0.w,X
            ror subtitleCharCompBuffer+1.w,X
            ror subtitleCharCompBuffer+2.w,X
            
            dey
            bne -
          inx
          inx
          inx
          cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
          bcc --
      
      ; NOTE: for future reference, here is the original shift routine
      ; before the more space-intensive loop unrolling was applied...
      ; apply right-shift to data
/*;      lda activeSubtitleXPos.w
      and #$0F
      beq @noRightShift
      cmp #$05
      bcc @rightShiftLeftTwo
      cmp #$09
      bcc @leftShiftLeftTwo
      cmp #$0D
      bcc @rightShiftRightTwo
      @leftShiftRightTwo:
        eor #$FF
        ina
        and #$03
        ; shift only the lines that are not empty
        clx
        --:
        tay
          -:
            asl subtitleCharCompBuffer+2.w,X
            rol subtitleCharCompBuffer+1.w,X
            dey
            bne -
          inx
          inx
          inx
          cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
          bcc --
        bra @noRightShift
      @rightShiftLeftTwo:
        and #$07
        beq @noRightShift
        ; shift only the lines that are not empty
        clx
        --:
        tay
          -:
            lsr subtitleCharCompBuffer+0.w,X
            ror subtitleCharCompBuffer+1.w,X
            dey
            bne -
          inx
          inx
          inx
          cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
          bcc --
        bra @noRightShift
      @leftShiftLeftTwo:
        and #$07
        beq @noRightShift
        eor #$FF
        ina
        and #$03
        ; shift only the lines that are not empty
        clx
        --:
        tay
          -:
            asl subtitleCharCompBuffer+1.w,X
            rol subtitleCharCompBuffer+0.w,X
            dey
            bne -
          inx
          inx
          inx
          cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
          bcc --
        bra @noRightShift
      @rightShiftRightTwo:
        and #$07
        beq @noRightShift
        ; shift only the lines that are not empty
        clx
        --:
        tay
          -:
            lsr subtitleCharCompBuffer+1.w,X
            ror subtitleCharCompBuffer+2.w,X
            dey
            bne -
          inx
          inx
          inx
          cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
          bcc -- */
      
      ; with max optimization
/*      ; apply right-shift to data
      lda activeSubtitleXPos.w
      and #$0F
      bne +
        jmp @noRightShift
      +:
      cmp #$05
      bcs +
        jmp @rightShiftLeftTwo
      +:
      cmp #$09
      bcs +
        jmp @leftShiftLeftTwo
      +:
      cmp #$0D
      bcs +
        jmp @rightShiftRightTwo
      +:
      @leftShiftRightTwo:
        eor #$FF
        ina
        and #$03
        dea
        bne +
          jmp @leftShiftRightTwo_shift1
        +:
        dea
        bne +
          jmp @leftShiftRightTwo_shift2
        +:
        @leftShiftRightTwo_shift3:
          leftShiftRightTwoLoop
        @leftShiftRightTwo_shift2:
          leftShiftRightTwoLoop
        @leftShiftRightTwo_shift1:
          leftShiftRightTwoLoop
        jmp @noRightShift
      @rightShiftLeftTwo:
        and #$07
        bne +
          jmp @noRightShift
        +:
        dea
        bne +
          jmp @rightShiftLeftTwo_shift1
        +:
        dea
        bne +
          jmp @rightShiftLeftTwo_shift2
        +:
        dea
        bne +
          jmp @rightShiftLeftTwo_shift3
        +:
        ; shift only the lines that are not empty
        @rightShiftLeftTwo_shift4:
          rightShiftLeftTwoLoop
        @rightShiftLeftTwo_shift3:
          rightShiftLeftTwoLoop
        @rightShiftLeftTwo_shift2:
          rightShiftLeftTwoLoop
        @rightShiftLeftTwo_shift1:
          rightShiftLeftTwoLoop
        jmp @noRightShift
      @leftShiftLeftTwo:
        and #$07
        bne +
          jmp @noRightShift
        +:
        eor #$FF
        ina
        and #$03
        
        dea
        bne +
          jmp @leftShiftLeftTwo_shift1
        +:
        dea
        bne +
          jmp @leftShiftLeftTwo_shift2
        +:
        @leftShiftLeftTwo_shift3:
          leftShiftLeftTwoLoop
        @leftShiftLeftTwo_shift2:
          leftShiftLeftTwoLoop
        @leftShiftLeftTwo_shift1:
          leftShiftLeftTwoLoop
        jmp @noRightShift
      @rightShiftRightTwo:
        and #$07
        bne +
          jmp @noRightShift
        +:
        dea
        bne +
          jmp @rightShiftRightTwo_shift1
        +:
        dea
        bne +
          jmp @rightShiftRightTwo_shift2
        +:
        dea
        bne +
          jmp @rightShiftRightTwo_shift3
        +:
        ; shift only the lines that are not empty
        @rightShiftRightTwoo_shift4:
          rightShiftRightTwoLoop
        @rightShiftRightTwo_shift3:
          rightShiftRightTwoLoop
        @rightShiftRightTwo_shift2:
          rightShiftRightTwoLoop
        @rightShiftRightTwo_shift1:
          rightShiftRightTwoLoop */
      
      @noRightShift:
      
      ; merge with current pattern
      lda #<subtitleCompBuffer
      sta newZpFreeReg+0
      lda #>subtitleCompBuffer
      sta newZpFreeReg+1
      
      clx
      ldy #(numSubtitleFontCharTopPaddingLines*2)
      -:
        ; get left pattern
        lda (newZpFreeReg),Y
        ; OR with char buffer
        ; (note that the endianness is swapped here
        ; to match the output sprite format)
        ora subtitleCharCompBuffer+1.w,X
        ; write back
        sta (newZpFreeReg),Y
        iny
        
        ; repeat for right pattern
        lda (newZpFreeReg),Y
        ora subtitleCharCompBuffer+0.w,X
        sta (newZpFreeReg),Y
        iny
        
        inx
        inx
        inx
        cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
        bcc -
    
    ; restore character index
    pla
    
    ; get char width and add to x-pos
    tax
    ; save currentX/16
    lda activeSubtitleXPos.w
    pha
      and #$F0
      sta @spanCheck+1.w
    pla
    clc
    adc ovlScene_fontWidthTable.w,X
    sta activeSubtitleXPos.w
    ; check currentX/16 against newX/16
    and #$F0
    ; self-modifying
    @spanCheck:
    cmp #$00
    ; if currentX/16 != newX/16, transfer spans two patterns
    ; (or goes exactly to end of current one)
    beq @noSpan
      
      ; note that there is an extra pattern at the end of the buffer
      ; to account for the rare possibility that every single available
      ; pattern is filled to capacity, and this routine attempts to
      ; initialize the "next" pattern (which nominally shouldn't exist)
      
      ; advance to next pattern
;      jsr advanceActiveSubtitleCompBufferPtrToNextPattern
      
;      sta spriteGrpGenAndTransferNeeded
      jsr sendNextSpriteBuffer
    @noSpan:
    
    rts
    
;  clearSubtitleCharCompBuffer:
;    tai blockClearWord,subtitleCharCompBuffer,subtitleCharCompBufferSize
;    rts
  
  turnSubsOff:
    stz subtitleDisplayOn.w
    
    ; FIXME?
    ; blank out the subtitles sprites from the sat
/*    lda currentSubtitleSpriteAttributeQueueSize.w
    beq @done
      ; set size of the area to blank
      lsr
      tax
      
      ; set write address
      st0 #$00
      st1 #<satVramAddr
      st2 #>satVramAddr
      
      st0 #$02
      @clearSizeCmd:
      jsr doVramClear
      
      ; initiate sat->satb dma
      st0 #$13
      st1 #<satVramAddr
      st2 #>satVramAddr*/
    @done:
    rts
  
  doVramClear:
    ; start write
;    cla
    -:
;      sta $0002.w
;      sta $0003.w
      st1 #$00
      st2 #$00
      dex
      bne -
    rts
    
  resetLineClearArea:
    tai blockClearWord,newLineClearAreaStart,newLineClearAreaEnd-newLineClearAreaStart
    rts
  
  resetStateForNewString:
    ; clear the clear area
;    tai blockClearWord,newLineClearAreaStart,newLineClearAreaEnd-newLineClearAreaStart
    jsr resetLineClearArea
    
    ; reset line count
/*    stz activeSubtitleLineNum.w
    ; reset total sprite count
    stz currentSubSpriteCount.w
    stz group1NonemptyLineCount.w
    stz group2NonemptyLineCount.w */
    tai blockClearWord,newStringClearAreaStart,newStringClearAreaEnd-newStringClearAreaStart
    
    ; reset attribute queue pointer
    lda subtitleDisplayQueueParity.w
    asl
    tax
    lda subtitleDisplayQueuePointerArray+0.w,X
    sta currentLineSpriteAttrStartPtr+0.w
    lda subtitleDisplayQueuePointerArray+1.w,X
    sta currentLineSpriteAttrStartPtr+1.w
    
    rts
    
  sendNextSpriteBufferAndForceAttributeSend:
;    lda #$FF
;    sta spriteAttrGenAndTransferNeeded.w
    ; should be safe
    inc spriteAttrGenAndTransferNeeded.w
    ; !!!!! drop through !!!!!
  sendNextSpriteBuffer:
    ; schedule graphics transfer
;    lda #$01
;    sta spriteGrpGenAndTransferNeeded.w
    inc spriteGrpGenAndTransferNeeded.w
    
    ; if adding this pattern makes the sprite 2 patterns wide,
    ; schedule an attribute transfer too
/*    lda nextSpriteAttrWidth.w
    ; assume a nonzero value is 1
    beq +
      sta spriteGrpGenAndTransferNeeded.w
      ; reset width to zero
      lda #$FF
    +:
    ina
    sta nextSpriteAttrWidth.w */
    
    rts
  
  doSpriteGrpGenAndTransfer:
    ; convert the active composition buffer to a sprite and send it
    ; to the next vram position
  
    ;=====
    ; do vram write
    ;=====
    
    lda nextSpriteAttrVramBase+0.w
    ; add in the current sprite width to the base position
    ; (i.e. offset position by 1 if this is the second pattern)
    clc
    adc nextSpriteAttrWidth.w
    sta @vramDstLowerCmd+1.w
    lda nextSpriteAttrVramBase+1.w
    adc #$00
    ; multiply tilenum by 64 to get vram address
    .rept 6
      asl @vramDstLowerCmd+1.w
      rol
    .endr
    sta @vramDstUpperCmd+1.w
    
    ; set vram dst
    st0 #$00
    @vramDstLowerCmd:
    ; self-modifying
    st1 #$00
    @vramDstUpperCmd:
    ; self-modifying
    st2 #$00
    
    ; start write
    st0 #$02
      
    @spritePlaneTransferCmd:
    tii subtitleCompBuffer,charShiftBufferA,bytesPerSpritePatternPlane
    tii charShiftBufferA,dropShadowBufferA,bytesPerSpritePatternPlane
    
    ; do first right shift
    ldx #(numSubtitleFontCharTopPaddingLines*2)
    -:
      lda charShiftBufferA+1.w,X
      lsr
      ora charShiftBufferB+1.w,X
      sta charShiftBufferA+1.w,X
      ora dropShadowBufferA+1.w,X
      ora dropShadowBufferB+1.w,X
      sta dropShadowBufferA+1.w,X
      
      lda charShiftBufferA+0.w,X
      ror
      sta charShiftBufferA+0.w,X
      ora dropShadowBufferA+0.w,X
      sta dropShadowBufferA+0.w,X
      
      cla
      ror
      sta charShiftBufferB+1.w,X
      sta dropShadowBufferB+1.w,X
      
      inx
      inx
      cpx #(bytesPerSpritePatternPlane-(numSubtitleFontCharBottomPaddingLines*2))
      bne -
    
    ; copy first plane
    tia charShiftBufferA,$0002,bytesPerSpritePatternPlane
    
    ; do second right shift
    ldx #(numSubtitleFontCharTopPaddingLines*2)
    -:
      lda charShiftBufferA+1.w,X
      lsr
      ora dropShadowBufferA+1.w,X
      sta dropShadowBufferA+1.w,X
      
      lda charShiftBufferA+0.w,X
      ror
      ora dropShadowBufferA+0.w,X
      sta dropShadowBufferA+0.w,X
      
      lda dropShadowBufferB+1.w,X
      ror
      sta dropShadowBufferB+1.w,X
      
      inx
      inx
      cpx #(bytesPerSpritePatternPlane-(numSubtitleFontCharBottomPaddingLines*2))
      bne -
      
      ; copy remaining planes
      tia dropShadowBufferA-2,$0002,bytesPerSpritePatternPlane
      tia dropShadowBufferA+0,$0002,bytesPerSpritePatternPlane
      ; note that this transfer is safe because dropShadowBufferA is
      ; immediately followed by dropShadowBufferB, whose first two
      ; bytes will always be zero (they're part of the top-row
      ; padding)
      tia dropShadowBufferA+2,$0002,bytesPerSpritePatternPlane
  
    ;=====
    ; update fields
    ;=====
    
    ; copy any overflow from the character composition process
    ; to the left side of the comp buffer, and clear the right side
    clx
;    cly
    ldy #(numSubtitleFontCharTopPaddingLines*2)
    lda #<subtitleCompBuffer
    sta newZpFreeReg+0
    lda #>subtitleCompBuffer
    sta newZpFreeReg+1
    -:
      ; note that we are swapping the endianness here
      
      ; clear right side of buffer
      cla
      sta (newZpFreeReg),Y
      iny
      lda subtitleCharCompBuffer+2.w,X
      sta (newZpFreeReg),Y
      iny
      
      inx
      inx
      inx
      cpx #(linesPerRawSceneFontChar*bytesPerSubtitleCharCompBufferLine)
      bcc -
    
    ; increment sprite width.
    ; if adding this pattern makes the sprite 2 patterns wide,
    ; schedule an attribute transfer
    lda nextSpriteAttrWidth.w
    ; assume a nonzero value is 1
    beq +
      sta spriteAttrGenAndTransferNeeded.w
    +:
    ina
    sta nextSpriteAttrWidth.w
    
    ; clear call flag
    stz spriteGrpGenAndTransferNeeded.w
    rts
  
  doSpriteAttrGenAndTransfer:
    ; get pointer to current attribute putpos
    lda currentLineSpriteCount.w
    asl
    asl
    asl
    clc
    adc currentLineSpriteAttrStartPtr+0.w
    sta newZpFreeReg+0
    cla
    adc currentLineSpriteAttrStartPtr+1.w
    sta newZpFreeReg+1
    
    ; set up fields
    ; Y (placeholder)
    lda activeSubtitleLineNum.w
    asl
    asl
    asl
    asl
    clc
    adc #spriteAttrBaseY
    sta (newZpFreeReg)
    ldy #$01
    cla
    adc #$00
    sta (newZpFreeReg),Y
    
    ; X (placeholder)
    iny
    lda currentLineSpriteCount.w
    ; *32 because everything should be double-width
    ; (except the last sprite, where it doesn't matter)
    asl
    asl
    asl
    asl
    asl
    clc
    ; offset 1 pixel to the left because after generating the outline,
    ; the subtitles will take up 2 more pixels on the right
    adc #spriteAttrBaseX-1
    sta (newZpFreeReg),Y
    iny
    cla
    adc #$00
    sta (newZpFreeReg),Y
    
    ; pattern
    iny
    lda nextSpriteAttrVramBase+0.w
    asl
    sta (newZpFreeReg),Y
    iny
    lda nextSpriteAttrVramBase+1.w
    rol
    sta (newZpFreeReg),Y
    
    ; flags
    ; low byte
    ; apply palette
    ; (note: top bit set = high priority)
    iny
    lda #$80
    ora currentSubtitlePaletteIndex.w
    sta (newZpFreeReg),Y
    iny
    ; high byte
    lda nextSpriteAttrWidth.w
    dea
    sta (newZpFreeReg),Y
    
    ; vram dstpos += 2
    ; (regardless of actual sprite width, because we need the start
    ; positions to be even in order to take advantage of double-width
    ; sprites)
/*    ldx #$02
    -:
      inc nextSpriteAttrVramBase+0.w
      bne +
        inc nextSpriteAttrVramBase+1.w
      +:
      dex
      bne - */
    lda #$02
    clc
    adc nextSpriteAttrVramBase+0.w
    sta nextSpriteAttrVramBase+0.w
    bcc +
      inc nextSpriteAttrVramBase+1.w
    +:
    
    ; reset sprite width
    stz nextSpriteAttrWidth.w
    
    ; increment line sprite count
    inc currentLineSpriteCount.w
    ; increment total sprite count
    inc currentSubSpriteCount.w
    
    ; clear call flag
    stz spriteAttrGenAndTransferNeeded.w
    rts
  
  ; this is called after the final transfer for the line has already
  ; been performed
  endCurrentLineComp:
    ;=====
    ; apply centering x-offset to current line's attributes
    ;=====
    
    ; centering offset = (256 - width) / 2
    lda activeSubtitleXPos.w
    ; empty line = no centering
    beq @noCenter
      eor #$FF
      ina
      lsr
      
      .ifdef extraSubtitleLeftOffsetFlag
        sec
        sbc #extraSubtitleLeftOffset
      .endif
      
      sta @xCenterOffset.w
      
      lda currentLineSpriteAttrStartPtr+0.w
      sta newZpFreeReg+0
      lda currentLineSpriteAttrStartPtr+1.w
      sta newZpFreeReg+1
      
      ldx currentLineSpriteCount.w
      ldy #02
      -:
        lda (newZpFreeReg),Y
        clc
        adc @xCenterOffset.w
        sta (newZpFreeReg),Y
        
        iny
        cla
        adc (newZpFreeReg),Y
        sta (newZpFreeReg),Y
        
        tya
        clc
        adc #$07
        tay
        
        dex
        bne -
    @noCenter:
    
    ;=====
    ; update and reset fields as needed
    ;=====
    
    ; update line attribute queue pointer
    lda currentLineSpriteCount.w
    asl
    asl
    asl
    clc
    adc currentLineSpriteAttrStartPtr+0.w
    sta currentLineSpriteAttrStartPtr+0.w
;    cla
;    adc currentLineSpriteAttrStartPtr+1.w
;    sta currentLineSpriteAttrStartPtr+1.w
    bcc +
      inc currentLineSpriteAttrStartPtr+1.w
    +:
    
    ;=====
    ; if line flood mode on, add garbage sprites offscreen until there are
    ; 16 sprites on the line
    ;=====
    
    lda lineFloodModeOn.w
    beq @noFlood
      lda currentLineSpriteCount.w
      ; empty line = no flood
      beq @noFlood
      cmp #lineFloodModeTargetSpriteCount
      bcs @noFlood
        tax
        
        lda currentLineSpriteAttrStartPtr+0.w
;        sta newZpFreeReg+0
        sta @floodInstr+3.w
        lda currentLineSpriteAttrStartPtr+1.w
;        sta newZpFreeReg+1
        sta @floodInstr+4.w
        
        ; set up y-pos
        lda activeSubtitleLineNum.w
        asl
        asl
        asl
        asl
        clc
        adc #spriteAttrBaseY
        sta @floodSpriteData+0.w
;        stz @floodSpriteData+3.w
        
        -:
          @floodInstr:
          tii @floodSpriteData,$0000,$0008
          
          lda @floodInstr+3.w
          clc
          adc #8
          sta @floodInstr+3.w
;          cla
;          adc @floodInstr+4.w
;          sta @floodInstr+4.w
          bcc +
            inc @floodInstr+4.w
          +:
          
          inc currentSubSpriteCount.w
          
          inx
          cpx #lineFloodModeTargetSpriteCount
          bne -
        
;        txa
;        sta currentLineSpriteCount.w
        
;        lda newZpFreeReg+0
        lda @floodInstr+3.w
        sta currentLineSpriteAttrStartPtr+0.w
;        lda newZpFreeReg+1
        lda @floodInstr+4.w
        sta currentLineSpriteAttrStartPtr+1.w
    @noFlood:
    
    ; reset fields that need to be reset
    jsr resetLineClearArea
    inc activeSubtitleLineNum.w
    
    rts
    
    @xCenterOffset:
      .db $00
    
    @floodSpriteData:
      ; double-width, x-pos -32, nothing else matters
      .dw $0000,$0000,$0000,$0100
  
  endCurrentSubComp:
    ;=====
    ; update and reset fields as needed
    ;=====
    
    ; set the attribute display start pointer
    lda subtitleDisplayQueueParity.w
    pha
      asl
      tax
      lda subtitleDisplayQueuePointerArray+0.w,X
      sta currentSubtitleSpriteAttributeQueuePtr+0.w
      sta newZpFreeReg+0
      lda subtitleDisplayQueuePointerArray+1.w,X
      sta currentSubtitleSpriteAttributeQueuePtr+1.w
      sta newZpFreeReg+1
      
      ; set the attribute queue size
      lda currentSubSpriteCount.w
      asl
      asl
      asl
      sta currentSubtitleSpriteAttributeQueueSize.w
    pla
    ; swap display parity
    ina
    and #$01
    sta subtitleDisplayQueueParity.w
    
    ;=====
    ; apply base y-offset
    ;=====
    
    ; multiply line count by 8, then subtract from base Y
    ; to center around target y-offset
;    lda activeSubtitleLineNum.w
    clx
    -:
      lda groupNonemptyLineCountArray.w,X
      asl
      asl
      asl
      sec
      sbc subtitleBaseY.w
      eor #$FF
      ina
      sta @yCenterOffsetArray.w,X
      
      .ifdef alignSubsTowardBorder
        lda groupNonemptyLineCountArray.w,X
        cmp #1
        bne @normalPositioningDone
          ; rather than centering,
          ; align 1-line groups toward the nearest border:
          ; bottom for group 0, top for group 1
          lda @yCenterOffsetArray.w,X
          cpx #1
          bne @group0Align
          @group1Align:
            sec
            sbc #spritePatternH/2
            bra @finishAlign
          @group0Align:
            clc
            adc #(spritePatternH/2)
          @finishAlign:
          sta @yCenterOffsetArray.w,X
        @normalPositioningDone:
        lda @yCenterOffsetArray.w,X
      .endif
      
      inx
      cpx #$02
      bne -
    
    ; HACK: shift group 2's lines to the top of the screen
    ; (we know the y-offset is 208, even though it's not
    ; actually defined as a constant, and we want it to
    ; instead be centered around 32)
    sec
    sbc #defaultSubtitleGroupTopToBottomGap
    sta @yCenterOffsetGroup2.w
    
    ; add this to the y-offset of everything in the queue
;    lda currentLineSpriteAttrStartPtr+0.w
;    sta newZpFreeReg+0
;    lda currentLineSpriteAttrStartPtr+1.w
;    sta newZpFreeReg+1
    
  ; lol this was a stupid idea
/*    lda groupNonemptyLineCountArray.w
    cmp #2
    bcs @noFloodOffset
      ldx currentSubSpriteCount.w
      ldy #4
      -:
        ; if target tile == 0, this is a flood filler sprite
        lda (newZpFreeReg),Y
        iny
        ora (newZpFreeReg),Y
        bne @noFloodAdjust
          ; width/height = 32px
          ; pos = 7
          iny
          iny
          lda #$11
          sta (newZpFreeReg),Y
          
          ; pos = 0
          tya
          clc
          adc #-7
          tay
          
          ; y-pos -= 8
          ; (only low byte matters at this point)
          lda (newZpFreeReg),Y
          clc
          adc #-8
          sta (newZpFreeReg),Y
          
          ; pos += 12
          tya
          clc
          adc #12
          bra @loopEnd
        @noFloodAdjust:
        ; pos += 7
        tya
        clc
        adc #7
        @loopEnd:
        tay
        dex
        bne -
    @noFloodOffset:*/
    
    ldx currentSubSpriteCount.w
    cly
    -:
      lda (newZpFreeReg),Y
      
      ; HACK: detect if group 1 (y >= $40) or group 2 (y >= $60)
      cmp #(spriteAttrBaseY+(spritePatternH*2))
      bcc +
        ; HACK: subtract off the extra offset, because the 8-bit range
        ; isn't enough to do this right
        sec
        sbc #$20
        sta (newZpFreeReg),Y
        
        lda @yCenterOffsetGroup2.w
        bra ++
      +:
        lda @yCenterOffsetGroup1.w
      ++:
      
      clc
      adc (newZpFreeReg),Y
      sta (newZpFreeReg),Y
      
      iny
      cla
      adc (newZpFreeReg),Y
      sta (newZpFreeReg),Y
      
      tya
      clc
      adc #$07
      tay
      
      dex
      bne -
    
    ; HACK: set flag in free memory area so raster crop
    ; knows whether to target 1-line or 2-line window
    ; HACK on top of HACK: this needs to propagate a frame
    ; late because of how the sprite refresh aligns with vsync,
    ; so we only set a "buffer" value here and copy it to the
    ; "real" rcrCropTargetsMultipleLines each frame the subtitle
    ; update code is called.
    ; this avoids 1-frame glitches when switching directly from
    ; 1-line to 2-line subtitles or vice versa.
    lda group1NonemptyLineCount.w
    dea
    sta nextRcrCropTargetsMultipleLines.w
    ; ...unless subtitles are off, in which case this is effective immediately
    lda subtitleDisplayOn.w
    bne +
      lda nextRcrCropTargetsMultipleLines.w
      sta rcrCropTargetsMultipleLines.w
    +:
    
    rts
    
    @yCenterOffsetArray:
    @yCenterOffsetGroup1:
      .db $00
    @yCenterOffsetGroup2:
      .db $00
;    @count:
;      .db $00
  
.ends

;=============================
; memory
;=============================

; place variables that need to be accessed from out-of-bank code
; in the fixed area
.bank fixedBank slot fixedSlot
.section "memory 1" free
  subtitleEngineOn:
    .db defaultSubtitleEngineOn
  rcrCropTargetsMultipleLines:
    .db $00
  nextRcrCropTargetsMultipleLines:
    .db $00
  ; this needs to be available to the rcr interrupt handler,
  ; and we don't want to waste time having it page in a new bank
  subtitleDisplayOn:
    .db $00
  prevSubtitleDisplayOn:
    .db $00
.ends

.bank freeBank slot freeSlot
.section "memory 2" free
  ; 0 = queue A is back (write), queue B is front (display)
  ; 1 = queue B is back, queue A is front
  subtitleDisplayQueueParity:
    .db $00
  subtitleDisplayQueuePointerArray:
    .dw subtitleSpriteAttributeQueueA
    .dw subtitleSpriteAttributeQueueB
  subtitleDisplayQueueSizeArray:
    .db $00
    .db $00
;  subtitleDisplayBackQueuePutPos:
;    .dw $0000
;  subtitleDisplayBackQueuePutPos:
;    .db $00
;  subtitleDisplayQueueCurrentX:
;    .db $00
  
  subtitleSpriteAttributeQueueA:
    .ds _sizeof_SpriteAttribute*maxNumSubtitleSprites,$00
  subtitleSpriteAttributeQueueB:
    .ds _sizeof_SpriteAttribute*maxNumSubtitleSprites,$00
  
  currentSubtitleSpriteAttributeQueuePtr:
    .dw $0000
  currentSubtitleSpriteAttributeQueueSize:
    .db $00
  
  subtitleScriptPtr:
;    .dw subtitleScriptData
    .dw defaultSubtitleScriptPtr
  
  queuedSubsOffTime:
    .dw $0000
  queuedSubsOffIsOn:
    .db $00
  
  lineFloodModeOn:
    .ifdef noDefaultLineFloodMode
      .db $00
    .else
      .db $FF
    .endif
  
;  subtitleSpriteClearNeeded:
;    .db $00
  
  ; reset to this at start of new subtitle
;  fontSpriteBaseVramTarget:
;    .dw $0000
  ; next converted font sprite goes here
;  fontSpriteNextVramTarget:
;    .dw $0000
  
;  subtitleBaseX:
;    .db 128
  subtitleBaseY:
    .db defaultSubtitleBaseY
  
  currentSubtitlePaletteIndex:
    .db $FF
  
  ; these fields need to be reset to zero at the start of a new string
  newLineClearAreaStart:
    ; these fields additionally need to be reset to zero
    ; at the start of a new line
;    newLineClearAreaStart:
      subtitleCharCompBuffer:
        .ds subtitleCharCompBufferSize,$00
      subtitleCharCompBufferEnd:
      
      subtitleCompBuffer:
        ; only the first composition buffer is cleared in the reset.
        ; the rest are cleared procedurally during the composition process
        .ds bytesPerSpritePatternPlane,$00
      subtitleCompBufferEnd:
    
      ; pre-padding to allow for single transfer of last 3 planes
      ; also used to clear out memory blocks with TAI
      dropShadowBufferAPrePad:
      blockClearWord:
        .dw $0000
      dropShadowBufferA:
        .ds bytesPerSpritePatternPlane,$00
      dropShadowBufferAEnd:
      
      dropShadowBufferB:
        .ds bytesPerSpritePatternPlane,$00
      dropShadowBufferBEnd:
      
      charShiftBufferA:
        .ds bytesPerSpritePatternPlane,$00
      charShiftBufferAEnd:
      charShiftBufferB:
        .ds bytesPerSpritePatternPlane,$00
      charShiftBufferBEnd:
      
      activeSubtitleXPos:
        .db $00
;    newLineClearAreaEnd:
      
    ; size in sprites of the next sprite attribute to be generated
    ; (1 or 2, or 0 before the first sprite is completed)
    nextSpriteAttrWidth:
      .db $00
    
    ; number of sprites from currentLineSpriteAttrStartPtr to attribute
    ; generator queue end
    currentLineSpriteCount:
      .db $00
    
    spriteGrpGenAndTransferNeeded:
      .db $00
    
    spriteAttrGenAndTransferNeeded:
      .db $00
  newLineClearAreaEnd:
  
  newStringClearAreaStart:
    activeSubtitleLineNum:
      .db $00
    currentSubSpriteCount:
      .db $00
    ; hack to get two groups of subtitles
    groupNonemptyLineCountArray:
      group1NonemptyLineCount:
        .db $00
      group2NonemptyLineCount:
        .db $00
/*    groupAttributeStartArray:
      .dw $0000
      .dw $0000
    groupAttributeSizeArray:
      .db $00
      .db $00 */
  newStringClearAreaEnd:
  
  
  ; tile ID of next sprite's base position in VRAM
  ; (add nextSpriteAttrWidth to get target for next graphics write)
  nextSpriteAttrVramBase:
    .dw $00
  
  ; pointer to start of current line's sprite attributes in the
  ; sprite attribute queue.
  ; when the end of the line is reached, we have to apply an
  ; x-offset from here to the end of the queue to center the sprites.
  currentLineSpriteAttrStartPtr:
    .dw $0000
.ends
;.endb
