
echo "*******************************************************************************"
echo "Updating build information..."
echo "*******************************************************************************"

buildStartTime=$(date)

# build counter for tracking just how many builds this goes through
if [ ! -e "build_counter.txt" ]; then
  printf "0" > "build_counter.txt"
fi

lastBuildNum=$(cat "build_counter.txt")
currentBuildNum=$((lastBuildNum + 1))
printf "$currentBuildNum" > "build_counter.txt"


if [ ! -e "build_log.txt" ]; then
  > "build_log.txt"
else
  printf "\\n\\n" >> "build_log.txt"
fi
printf "Build $currentBuildNum\\n  Started:   $buildStartTime" >> "build_log.txt"

echo "*******************************************************************************"
echo "Setting up environment..."
echo "*******************************************************************************"

set -o errexit

BASE_PWD=$PWD
PATH=".:$PATH"
INROM="tenma_02.iso"
OUTROM="tenma_02_build.iso"
WLADX="./wla-dx/binaries/wla-huc6280"
WLALINK="./wla-dx/binaries/wlalink"
DISCASTER="../discaster/discaster"

function remapPalette() {
  oldFile=$1
  palFile=$2
  newFile=$3
  
  convert "$oldFile" -dither None -remap "$palFile" PNG32:$newFile
}

function remapPaletteOverwrite() {
  newFile=$1
  palFile=$2
  
  remapPalette $newFile $palFile $newFile
}

mkdir -p out

echo "********************************************************************************"
echo "Building project tools..."
echo "********************************************************************************"

make blackt
make libpce
make

if [ ! -f $WLADX ]; then
  
  echo "********************************************************************************"
  echo "Building WLA-DX..."
  echo "********************************************************************************"
  
  cd wla-dx
    cmake -G "Unix Makefiles" .
    make
  cd $BASE_PWD
  
fi

echo "*******************************************************************************"
echo "Copying binaries..."
echo "*******************************************************************************"

cp -r base out
cp -r rsrc_raw out

cp "$INROM" "$OUTROM"

echo "*******************************************************************************"
echo "Building font..."
echo "*******************************************************************************"

#numFontChars=96
##numLimitedFontChars=80
#bytesPerFontChar=10

mkdir -p out/font
fontbuild "font/" "out/font/font.bin" "out/font/fontwidth.bin"
fontbuild "font/scene/" "out/font/font_scene.bin" "out/font/fontwidth_scene.bin"
fontbuild "font/narrow/" "out/font/font_narrow.bin" "out/font/fontwidth_narrow.bin"
fontbuild "font/narrower/" "out/font/font_narrower.bin" "out/font/fontwidth_narrower.bin"
fontbuild "font/tall/" "out/font/font_tall.bin" "out/font/fontwidth_tall.bin" 16

echo "*******************************************************************************"
echo "Building script..."
echo "*******************************************************************************"

mkdir -p out/scripttxt
mkdir -p out/scriptwrap
mkdir -p out/script

tenma_scriptimport

tenma_scriptwrap "out/scripttxt/spec_battle.txt" "out/scriptwrap/spec_battle.txt"
tenma_scriptwrap "out/scripttxt/spec_boss.txt" "out/scriptwrap/spec_boss.txt"
tenma_scriptwrap "out/scripttxt/spec_bossalt.txt" "out/scriptwrap/spec_bossalt.txt"
tenma_scriptwrap "out/scripttxt/spec_credits.txt" "out/scriptwrap/spec_credits.txt"
tenma_scriptwrap "out/scripttxt/spec_dialogue.txt" "out/scriptwrap/spec_dialogue.txt"
tenma_scriptwrap "out/scripttxt/spec_enemy.txt" "out/scriptwrap/spec_enemy.txt"
tenma_scriptwrap "out/scripttxt/spec_generic_text.txt" "out/scriptwrap/spec_generic_text.txt"
tenma_scriptwrap "out/scripttxt/spec_info.txt" "out/scriptwrap/spec_info.txt"
tenma_scriptwrap "out/scripttxt/spec_intro_ending.txt" "out/scriptwrap/spec_intro_ending.txt"
tenma_scriptwrap "out/scripttxt/spec_items.txt" "out/scriptwrap/spec_items.txt"
#tenma_scriptwrap "out/scripttxt/spec_menu.txt" "out/scriptwrap/spec_menu.txt"
cp "out/scripttxt/spec_menu.txt" "out/scriptwrap/spec_menu.txt"
tenma_scriptwrap "out/scripttxt/spec_misc.txt" "out/scriptwrap/spec_misc.txt"
tenma_scriptwrap "out/scripttxt/spec_saveload.txt" "out/scriptwrap/spec_saveload.txt"
tenma_scriptwrap "out/scripttxt/spec_shop.txt" "out/scriptwrap/spec_shop.txt"
tenma_scriptwrap "out/scripttxt/spec_subtitle.txt" "out/scriptwrap/spec_subtitle.txt" "table/tenma_scenes_en.tbl" "out/font/fontwidth_scene.bin" 0x20 1

#cp "rsrc_raw/script_dictionary.bin" "out/script"
tenma_scriptbuild "out/scriptwrap/" "out/script/"

echo "*******************************************************************************"
echo "Building graphics..."
echo "*******************************************************************************"

mkdir -p out/grp
mkdir -p out/maps

for file in tilemappers/*.txt; do
  tilemapper_pce "$file"
done;

grpundmp_pce "rsrc/grp/interface3.png" 0xE0 "out/grp/interface3.bin"

spriteundmp_pce "rsrc/grp/gamble_dice.png" "out/grp/gamble_dice.bin" -r 8 -n 0x18

grpundmp_pce "rsrc/grp/gamble_bg.png" 0x180 "out/grp/gamble_bg.bin" -r 32

grpundmp_pce "rsrc/grp/signs.png" 24 "out/grp/signs.bin" -r 4 --notrans -p "rsrc_raw/pal/edo_main_signs.pal"
grpdmp_pce "out/grp/signs.bin" "out/grp/signs_gray.png" -r 4 --notrans
tenma_signgen "out/grp/signs_gray.png" 0x00 0x00 "out/grp/sign_0"
tenma_signgen "out/grp/signs_gray.png" 0x10 0x00 "out/grp/sign_1"
tenma_signgen "out/grp/signs_gray.png" 0x00 0x10 "out/grp/sign_2"
tenma_signgen "out/grp/signs_gray.png" 0x10 0x10 "out/grp/sign_3"
tenma_signgen "out/grp/signs_gray.png" 0x00 0x20 "out/grp/sign_4"
tenma_signgen "out/grp/signs_gray.png" 0x10 0x20 "out/grp/sign_5"

spriteundmp_pce "rsrc/grp/pyramid_n.png" "out/grp/pyramid_n.bin" -r 1 -n 1

spriteundmp_pce "rsrc/grp/stonemason_bonus.png" "out/grp/stonemason_bonus.bin" -r 2 -n 2

#=====================
# title
#=====================

grpdmp_pce "out/rsrc_raw/grp/title_vram_bg_6000.bin" "out/grp/title_vram_bg_6000.png" --notrans
grpdmp_pce "out/rsrc_raw/grp/title_vram_bg_8000.bin" "out/grp/title_vram_bg_8000.png" --notrans

decolorize_pce "rsrc/grp/title_logo.png" "rsrc_raw/pal/title_logo_mod.pal" "out/grp/title_logo_gray.png"

convert "out/grp/title_vram_bg_6000.png"\
  \( "out/grp/title_logo_gray.png" -crop 32x32+128+0 +repage \) -geometry +80+0 -compose Copy -composite\
  \( "out/grp/title_logo_gray.png" -crop 128x40+0+0 +repage \) -geometry +0+32 -compose Copy -composite\
  PNG32:out/grp/title_vram_bg_6000.png

convert "out/grp/title_vram_bg_8000.png"\
  \( "out/grp/title_logo_gray.png" -crop 32x8+128+32 +repage \) -geometry +0+192 -compose Copy -composite\
  PNG32:out/grp/title_vram_bg_8000.png

grpundmp_pce_new "out/grp/title_vram_bg_6000.png" "out/rsrc_raw/grp/title_vram_bg_6000.bin" --notrans
grpundmp_pce_new "out/grp/title_vram_bg_8000.png" "out/rsrc_raw/grp/title_vram_bg_8000.bin" --notrans

#=====================
# intro credits
#=====================

remapPalette "rsrc/grp/intro_cred.png" "rsrc/grp/intro_cred_remap.png" "out/grp/intro_cred.png"
spritebuild_pce "out/grp/intro_cred.png" "rsrc/grp/intro_cred.txt" "rsrc_raw/pal/intro_cred_sprite.pal" "out/grp/intro_cred_raw.bin"
tenma_spriteprep "out/grp/intro_cred_sprite.bin"

datpatch "out/rsrc_raw/grp/title_vram_sprites_2e00.bin" "out/rsrc_raw/grp/title_vram_sprites_2e00.bin" "out/grp/intro_cred_raw.bin" 0x0 0x0 0x1200
# copy last 8 sprites to unused sprites for title messages
datpatch "out/rsrc_raw/grp/title_vram_sprites_7200.bin" "out/rsrc_raw/grp/title_vram_sprites_7200.bin" "out/grp/intro_cred_raw.bin" 0x300 0x1200 0x400
#datpatch "out/rsrc_raw/grp/title_vram_sprites_7200.bin" "out/rsrc_raw/grp/title_vram_sprites_7200.bin" "out/grp/intro_cred_raw.bin" 0xB00 0x1400 0x200

#=====================
# plans
#=====================

remapPalette "rsrc/grp/plans.png" "rsrc/grp/orig/plans.png" "out/grp/plans.png"
grpundmp_pce_new "out/grp/plans.png" "out/rsrc_raw/grp/plans.bin" -p rsrc_raw/pal/plans.pal --notrans -r 32

#=====================
# omake
#=====================

remapPalette "rsrc/grp/omake.png" "rsrc/grp/orig/omake.png" "out/grp/omake.png"
grpundmp_pce_new "out/grp/omake.png" "out/rsrc_raw/grp/omake.bin" -p rsrc_raw/pal/omake.pal --notrans -r 32

#=====================
# "the end"
#=====================

#cp "rsrc/grp/theend.png" "out/grp/theend.png"
remapPalette "rsrc/grp/theend.png" "rsrc/grp/theend_remap.png" "out/grp/theend.png"
remapPalette "rsrc/grp/theend_fade_0.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_0.png"
remapPalette "rsrc/grp/theend_fade_1.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_1.png"
remapPalette "rsrc/grp/theend_fade_2.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_2.png"
remapPalette "rsrc/grp/theend_fade_3.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_3.png"
remapPalette "rsrc/grp/theend_fade_4.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_4.png"
remapPalette "rsrc/grp/theend_fade_5.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_5.png"
remapPalette "rsrc/grp/theend_fade_6.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_6.png"
remapPalette "rsrc/grp/theend_fade_7.png" "rsrc/grp/theend_remap.png" "out/grp/theend_fade_7.png"

spritebuild_pce "out/grp/theend.png" "rsrc/grp/theend.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_kanji_raw.bin"
spritebuild_pce "out/grp/theend_fade_0.png" "rsrc/grp/theend_fade_0.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_0_raw.bin"
spritebuild_pce "out/grp/theend_fade_1.png" "rsrc/grp/theend_fade_1.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_1_raw.bin"
spritebuild_pce "out/grp/theend_fade_2.png" "rsrc/grp/theend_fade_2.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_2_raw.bin"
spritebuild_pce "out/grp/theend_fade_3.png" "rsrc/grp/theend_fade_3.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_3_raw.bin"
spritebuild_pce "out/grp/theend_fade_4.png" "rsrc/grp/theend_fade_4.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_4_raw.bin"
spritebuild_pce "out/grp/theend_fade_5.png" "rsrc/grp/theend_fade_5.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_5_raw.bin"
spritebuild_pce "out/grp/theend_fade_6.png" "rsrc/grp/theend_fade_6.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_6_raw.bin"
spritebuild_pce "out/grp/theend_fade_7.png" "rsrc/grp/theend_fade_7.txt" "rsrc_raw/pal/theend_sprite_mod.pal" "out/grp/theend_fade_7_raw.bin"

# graphics:
# concatenate all together
cat "out/grp/theend_kanji_raw.bin" "out/grp/theend_fade_0_raw.bin" "out/grp/theend_fade_1_raw.bin" "out/grp/theend_fade_2_raw.bin" "out/grp/theend_fade_3_raw.bin" "out/grp/theend_fade_4_raw.bin" "out/grp/theend_fade_5_raw.bin" "out/grp/theend_fade_6_raw.bin" "out/grp/theend_fade_7_raw.bin" > "out/grp/theend_all_raw.bin"

# sprite states:
# concatenate each fade state with sprites for kanji, then convert
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_0.bin" > "out/grp/theend_fade0_sprites_raw.bin"
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_1.bin" > "out/grp/theend_fade1_sprites_raw.bin"
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_2.bin" > "out/grp/theend_fade2_sprites_raw.bin"
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_3.bin" > "out/grp/theend_fade3_sprites_raw.bin"
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_4.bin" > "out/grp/theend_fade4_sprites_raw.bin"
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_5.bin" > "out/grp/theend_fade5_sprites_raw.bin"
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_6.bin" > "out/grp/theend_fade6_sprites_raw.bin"
cat "out/grp/theend_kanji.bin" "out/grp/theend_fade_7.bin" > "out/grp/theend_fade7_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade0_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade1_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade2_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade3_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade4_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade5_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade6_sprites_raw.bin"
tenma_spriteprep "out/grp/theend_fade7_sprites_raw.bin"

#=====================
# theater credits
#=====================

remapPalette "rsrc/grp/theater_credits.png" "rsrc/grp/orig/theater_credits.png" "out/grp/theater_credits.png"
spritebuild_pce "out/grp/theater_credits.png" "rsrc/grp/theater_credits.txt" "rsrc_raw/pal/theater_credits_sprite_mod.pal" "out/grp/theater_credits_raw.bin"
datsnip "out/grp/theater_credits_raw.bin" 0x0 0x2000 "out/grp/theater_credits_raw_1.bin"
datsnip "out/grp/theater_credits_raw.bin" 0x2000 0x2000 "out/grp/theater_credits_raw_2.bin"
tenma_spriteprep "out/grp/theater_credits_sprite.bin"

#=====================
# theater end
#=====================

cp "rsrc/grp/theater_end.png" "out/grp/theater_end.png"
spritebuild_pce "out/grp/theater_end.png" "rsrc/grp/theater_end.txt" "rsrc_raw/pal/theater_credits_sprite.pal" "out/grp/theater_end_raw.bin"
# copy output to last 4 tiles of data, corresponding to tiles 0x1FC-0x1FF
datpatch "out/grp/theater_credits_raw_2.bin" "out/grp/theater_credits_raw_2.bin" "out/grp/theater_end_raw.bin" 0x1E00
tenma_spriteprep "out/grp/theater_end_sprite.bin"

#=====================
# theater acts
#=====================

# remapPalette "rsrc/grp/theater_acts_1.png" "rsrc/grp/theater_acts_remap.png" "out/grp/theater_acts_1.png"
# remapPalette "rsrc/grp/theater_acts_2.png" "rsrc/grp/theater_acts_remap.png" "out/grp/theater_acts_2.png"
# remapPalette "rsrc/grp/theater_acts_3.png" "rsrc/grp/theater_acts_remap.png" "out/grp/theater_acts_3.png"
remapPalette "rsrc/grp/theater_acts_1.png" "rsrc/grp/theend_remap.png" "out/grp/theater_acts_1.png"
remapPalette "rsrc/grp/theater_acts_2.png" "rsrc/grp/theend_remap.png" "out/grp/theater_acts_2.png"
remapPalette "rsrc/grp/theater_acts_3.png" "rsrc/grp/theend_remap.png" "out/grp/theater_acts_3.png"

# spritebuild_pce "out/grp/theater_acts_1.png" "rsrc/grp/theater_acts_1.txt" "rsrc_raw/pal/theater_acts_sprite.pal" "out/grp/theater_acts_1_raw.bin"
# spritebuild_pce "out/grp/theater_acts_2.png" "rsrc/grp/theater_acts_2.txt" "rsrc_raw/pal/theater_acts_sprite.pal" "out/grp/theater_acts_2_raw.bin"
# spritebuild_pce "out/grp/theater_acts_3.png" "rsrc/grp/theater_acts_3.txt" "rsrc_raw/pal/theater_acts_sprite.pal" "out/grp/theater_acts_3_raw.bin"
spritebuild_pce "out/grp/theater_acts_1.png" "rsrc/grp/theater_acts_1.txt" "rsrc_raw/pal/theater_acts_sprite_mod.pal" "out/grp/theater_acts_1_raw.bin"
spritebuild_pce "out/grp/theater_acts_2.png" "rsrc/grp/theater_acts_2.txt" "rsrc_raw/pal/theater_acts_sprite_mod.pal" "out/grp/theater_acts_2_raw.bin"
spritebuild_pce "out/grp/theater_acts_3.png" "rsrc/grp/theater_acts_3.txt" "rsrc_raw/pal/theater_acts_sprite_mod.pal" "out/grp/theater_acts_3_raw.bin"
tenma_spriteprep "out/grp/theater_acts_1_sprite.bin"
tenma_spriteprep "out/grp/theater_acts_2_sprite.bin"
tenma_spriteprep "out/grp/theater_acts_3_sprite.bin"

#=====================
# theater title
#=====================

datsnip "out/grp/theater_title.bin" 0x0 0x2000 "out/grp/theater_title_1.bin"
datsnip "out/grp/theater_title.bin" 0x2000 0x2000 "out/grp/theater_title_2.bin"

datsnip "rsrc_raw/pal/theater_acts_sprite_mod.pal" 0x2 0x1E "out/rsrc_raw/pal/theater_acts_sprite_mod_cropped.pal"

#=====================
# scd error
#=====================

datpad_double "out/maps/scderror.bin" 0x1000 0x0
cat "out/maps/scderror.bin" "out/grp/scderror.bin" > "out/grp/scderror_vram.bin"
datpad_double "out/grp/scderror_vram.bin" 0x5000 0x0

echo "********************************************************************************"
echo "Applying ASM patches..."
echo "********************************************************************************"

function applyAsmPatch() {
  infile=$1
  asmname=$2
  prelinked=$3
  
  if [ "$prelinked" = "" ]; then
    prelinked=0
  fi
  
  infile_base=$(basename $infile)
  infile_base_noext=$(basename $infile .bin)
  
  linkfile=${asmname}_link
  
  echo "******************************"
  echo "patching: $asmname"
  echo "******************************"
  
  # generate linkfile
  printf "[objects]\n${asmname}.o" >"asm/$linkfile"
  
  cp "$infile" "asm/$infile_base"
  
  cd asm
    # apply hacks
    ../$WLADX -I ".." -D ROMNAME_BASE="${infile_base_noext}" -D ROMNAME="${infile_base}" -D ROMNAME_GEN_INC="gen/${infile_base_noext}.inc" -D PRELINKED=${prelinked} -o "$asmname.o" "$asmname.s"
    ../$WLALINK -v -S "$linkfile" "${infile_base}_build"
  cd $BASE_PWD
  
  mv -f "asm/${infile_base}_build" "out/base/${infile_base}"
  rm "asm/${infile_base}"
  
  rm asm/*.o
  
  # delete linkfile
  rm "asm/$linkfile"
}

function applyAsmPatchScene() {
  infile=$1
  asmfile=$2
  infile_base=$(basename $infile)
  asmfile_base=$(basename $asmfile)
  asmfile_noext=$(basename $asmfile .s)
  
#  echo $infile $asmfile $infile_base $asmfile_base
  
  cp "$asmfile" "asm"
#  cp "$infile" "asm"
    applyAsmPatch "$infile" "$asmfile_noext"
  rm -f "asm/$asmfile_base"
#  mv -f "asm/$infile_base" "$infile"
  mv -f "out/base/$infile_base" "$infile"
}

rm -rf out/metabank
cp -r out/base out/metabank
> "out/base/blank.bin"

datpad_double "out/metabank/empty_2.bin" 0x10000 0x0000
datpad_double "out/metabank/kernel_9.bin" 0x10000 0x4000
datpad_double "out/metabank/overw_13.bin" 0x10000 0x9000
datpad_double "out/metabank/shop_19.bin" 0x10000 0xC000
datpad_double "out/metabank/battle_1D.bin" 0x10000 0x9000
# FIXME: check load addr
datpad_double "out/metabank/unknown1_23.bin" 0x10000 0x0000
# FIXME: check load addr
datpad_double "out/metabank/battle_extra_27.bin" 0x10000 0xC000
# FIXME: check load addr
datpad_double "out/metabank/battle_text_2D.bin" 0x10000 0xC000
# FIXME: check load addr
datpad_double "out/metabank/battle_text2_31.bin" 0x10000 0xC000
# FIXME: check load addr
datpad_double "out/metabank/unknown2_35.bin" 0x10000 0x0000
datpad_double "out/metabank/intro_3B.bin" 0x10000 0x9000
datpad_double "out/metabank/saveload_45.bin" 0x10000 0x9000
# FIXME: check load addr
datpad_double "out/metabank/generic_text_4B.bin" 0x10000 0xC000
# FIXME: check load addr
datpad_double "out/metabank/credits_4F.bin" 0x10000 0xC000
datpad_double "out/metabank/special_51.bin" 0x10000 0x9000
# FIXME: check load addr
datpad_double "out/metabank/unknown3_57.bin" 0x10000 0x0000
# FIXME: check load addr
datpad_double "out/metabank/info_5B.bin" 0x10000 0xC000
datpad_double "out/metabank/masakado_5F.bin" 0x10000 0xC000
datpad_double "out/base/blank.bin" 0x10000 0xC000 "out/metabank/new1.bin"
datpad_double "out/base/blank.bin" 0x10000 0xC000 "out/metabank/new2.bin"
datpad_double "out/base/blank.bin" 0x10000 0xC000 "out/metabank/new3.bin"
datpad_double "out/base/blank.bin" 0x10000 0xC000 "out/metabank/new4.bin"
cat "out/metabank/empty_2.bin" "out/metabank/kernel_9.bin" "out/metabank/overw_13.bin" "out/metabank/shop_19.bin" "out/metabank/battle_1D.bin" "out/metabank/unknown1_23.bin" "out/metabank/battle_extra_27.bin" "out/metabank/battle_text_2D.bin" "out/metabank/battle_text2_31.bin" "out/metabank/unknown2_35.bin" "out/metabank/intro_3B.bin" "out/metabank/saveload_45.bin" "out/metabank/generic_text_4B.bin" "out/metabank/credits_4F.bin" "out/metabank/special_51.bin" "out/metabank/unknown3_57.bin" "out/metabank/info_5B.bin" "out/metabank/masakado_5F.bin" "out/metabank/new1.bin" "out/metabank/new2.bin" "out/metabank/new3.bin" "out/metabank/new4.bin" > "out/base/main_asm.bin"

applyAsmPatch "out/base/main_asm.bin" "main"
# as part of a complicated scheme to cache both the battle and overw modules
# entirely within memory, parts of the finalized overw binary need to be stored
# in the new1 and new2 units. to make that happen, we do a poor man's "compile"
# and "link" steps, first building a binary with placeholders for those sections,
# then doing a second pass that inserts that finalized versions. 
datpatch "out/base/overw_13.bin" "out/base/overw_13.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*2)+0x9000))
applyAsmPatch "out/base/main_asm.bin" "main" 1

datpatch "out/base/empty_2.bin" "out/base/empty_2.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*0)+0x0000))
datpatch "out/base/kernel_9.bin" "out/base/kernel_9.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*1)+0x4000))
datpatch "out/base/overw_13.bin" "out/base/overw_13.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*2)+0x9000))
datpatch "out/base/shop_19.bin" "out/base/shop_19.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*3)+0xC000))
datpatch "out/base/battle_1D.bin" "out/base/battle_1D.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*4)+0x9000))
datpatch "out/base/unknown1_23.bin" "out/base/unknown1_23.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*5)+0x0000))
datpatch "out/base/battle_extra_27.bin" "out/base/battle_extra_27.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*6)+0xC000))
datpatch "out/base/battle_text_2D.bin" "out/base/battle_text_2D.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*7)+0xC000))
datpatch "out/base/battle_text2_31.bin" "out/base/battle_text2_31.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*8)+0xC000))
datpatch "out/base/unknown2_35.bin" "out/base/unknown2_35.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*9)+0x0000))
datpatch "out/base/intro_3B.bin" "out/base/intro_3B.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*10)+0x9000))
datpatch "out/base/saveload_45.bin" "out/base/saveload_45.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*11)+0x9000))
datpatch "out/base/generic_text_4B.bin" "out/base/generic_text_4B.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*12)+0xC000))
datpatch "out/base/credits_4F.bin" "out/base/credits_4F.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*13)+0xC000))
datpatch "out/base/special_51.bin" "out/base/special_51.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*14)+0x9000))
datpatch "out/base/unknown3_57.bin" "out/base/unknown3_57.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*15)+0x0000))
datpatch "out/base/info_5B.bin" "out/base/info_5B.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*16)+0xC000))
datpatch "out/base/masakado_5F.bin" "out/base/masakado_5F.bin" "out/base/main_asm.bin" 0x0 $(((0x10000*17)+0xC000))
datsnip "out/base/main_asm.bin" $(((0x10000*18)+0xC000)) 0x2000 "out/base/new1.bin"
datsnip "out/base/main_asm.bin" $(((0x10000*19)+0xC000)) 0x1800 "out/base/new2.bin"
datsnip "out/base/main_asm.bin" $(((0x10000*20)+0xC000)) 0x2000 "out/base/new3.bin"
datsnip "out/base/main_asm.bin" $(((0x10000*21)+0xC000)) 0x1000 "out/base/new4.bin"

# copy the contents of info to the last 3 sectors of unknown1, which are not
# used.
# this will cause it to get loaded with the battle module cache,
# so we can load it without having to read from the cd.
datpatch "out/base/unknown1_23.bin" "out/base/unknown1_23.bin" "out/base/info_5B.bin" 0x800 0x0 0x1800

echo "********************************************************************************"
echo "Patching disc..."
echo "********************************************************************************"

tenma_patch "$OUTROM" "$OUTROM"

echo "*******************************************************************************"
echo "Success!"
echo "Output file:" $OUTROM
echo "*******************************************************************************"

buildEndTime=$(date)
printf "\\n  Completed: $buildEndTime" >> "build_log.txt"
