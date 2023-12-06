set -o errexit

mkdir -p rsrc/grp/orig
mkdir -p rsrc_raw/grp

make

# ./grpunmap_pce rsrc_raw/grp/title_logo_grp.bin rsrc_raw/grp/title_logo_map.bin 64 32 rsrc/grp/orig/title_logo.png -v 0x100 -p rsrc_raw/pal/title_logo.pal -t
# ./grpunmap_pce rsrc_raw/grp/title_logo_grp.bin rsrc_raw/grp/title_logo_map.bin 64 32 rsrc/grp/orig/title_logo_grayscale.png -v 0x100 -t
# ./grpunmap_pce "rsrc_raw/grp/loading_grp.bin" "rsrc_raw/map/loading.map" 64 64 "rsrc/grp/orig/loading.png" -v 0x0 -p "rsrc_raw/pal/loading.pal"
# ./grpdmp_pce "rsrc_raw/grp/dungeon_logo.bin" "rsrc/grp/orig/dungeon_logo.png" -p "rsrc_raw/pal/dungeon_logo_line.pal" -r 16
# ./grpdmp_pce "rsrc_raw/grp/button_yesno.bin" "rsrc/grp/orig/button_yesno.png" -p "rsrc_raw/pal/dungeon_button_line.pal" -r 6
# ./grpdmp_pce "rsrc_raw/grp/button_onoff.bin" "rsrc/grp/orig/button_onoff.png" -p "rsrc_raw/pal/dungeon_button_line.pal" -r 6
# 
# ./spritedmp_pce "rsrc_raw/grp/mrflea.bin" "rsrc/grp/orig/mrflea.png" -p "rsrc_raw/pal/dungeon_mrflea_line.pal" -r 16
# 
# ./spritedmp_pce "rsrc_raw/grp/bayoen.bin" "rsrc/grp/orig/bayoen_ba.png" -p "rsrc_raw/pal/dungeon_bayoen_line.pal" -r 2 -s 0x0
# ./spritedmp_pce "rsrc_raw/grp/bayoen.bin" "rsrc/grp/orig/bayoen_yo.png" -p "rsrc_raw/pal/dungeon_bayoen_line.pal" -r 2 -s 0x800
# ./spritedmp_pce "rsrc_raw/grp/bayoen.bin" "rsrc/grp/orig/bayoen_e.png" -p "rsrc_raw/pal/dungeon_bayoen_line.pal" -r 2 -s 0x400
# ./spritedmp_pce "rsrc_raw/grp/bayoen.bin" "rsrc/grp/orig/bayoen_n.png" -p "rsrc_raw/pal/dungeon_bayoen_line.pal" -r 2 -s 0xC00
# 
# ./grpunmap_pce "rsrc_raw/grp/error_accard.bin" "rsrc_raw/grp/error_accard.bin" 64 64 "rsrc/grp/orig/error_accard.png" -v 0x0 -p "rsrc_raw/pal/error_accard.pal"
# ./grpunmap_pce "rsrc_raw/grp/error_clearbak.bin" "rsrc_raw/grp/error_clearbak.bin" 64 64 "rsrc/grp/orig/error_clearbak.png" -v 0x0 -p "rsrc_raw/pal/error_clearbak.pal"
# ./grpunmap_pce "rsrc_raw/grp/error_clearfiles.bin" "rsrc_raw/grp/error_clearfiles.bin" 64 64 "rsrc/grp/orig/error_clearfiles.png" -v 0x0 -p "rsrc_raw/pal/error_clearfiles.pal"

#./yuna_grpextr "rsrc_raw/img/karaoke.bin" "rsrc/grp/orig/karaoke.png"
#./yuna_grpextr "rsrc_raw/img/snap.bin" "rsrc/grp/orig/snap.png"

#./datsnip tenma_02.iso 0x6D01000 0x10000 "rsrc_raw/grp/title.bin"

# ./datsnip tenma_02.iso 0x3B000 0x2000 "rsrc_raw/grp/interface1.bin"
# ./datsnip tenma_02.iso 0x55000 0x2000 "rsrc_raw/grp/interface2.bin"
# ./datsnip tenma_02.iso 0x6D02200 0x1C00 "rsrc_raw/grp/interface3.bin"
# ./grpdmp_pce "rsrc_raw/grp/interface1.bin" "rsrc/grp/orig/interface1.png"
# ./grpdmp_pce "rsrc_raw/grp/interface2.bin" "rsrc/grp/orig/interface2.png"
# ./grpdmp_pce "rsrc_raw/grp/interface3.bin" "rsrc/grp/orig/interface3.png"
# # ./datsnip tenma_02.iso 0x3B000 0x10000 "rsrc_raw/grp/interface1.bin"
# # ./datsnip tenma_02.iso 0x55000 0x10000 "rsrc_raw/grp/interface2.bin"
# # ./datsnip tenma_02.iso 0x6D01000 0x10000 "rsrc_raw/grp/interface3.bin"
# # ./spritedmp_pce "rsrc_raw/grp/interface1.bin" "test1.png"
# # ./spritedmp_pce "rsrc_raw/grp/interface2.bin" "test2.png"
# # ./spritedmp_pce "rsrc_raw/grp/interface3.bin" "test3.png"

# ./datsnip tenma_02.iso 0x6CA5000 0xC00 "rsrc_raw/grp/gamble_dice.bin"
# ./spritedmp_pce "rsrc_raw/grp/gamble_dice.bin" "rsrc/grp/orig/gamble_dice.png" -n 0x18 -r 8

# ./datsnip tenma_02.iso 0x6CA1000 0x3000 "rsrc_raw/grp/gamble_bg.bin"
# ./grpdmp_pce "rsrc_raw/grp/gamble_bg.bin" "rsrc/grp/orig/gamble_bg.png" -r 32

# ./tenma_signscan tenma_02.iso rsrc/signs_gray.png

# ./spritedmp_pce "rsrc_raw/grp/pyramid_n.bin" "rsrc/grp/orig/pyramid_n.png" -n 1 -r 1

# ./datsnip tenma_02.iso 0x75E3800 0x3000 "rsrc_raw/grp/plans.bin"
# ./grpdmp_pce "rsrc_raw/grp/plans.bin" "rsrc/grp/orig/plans.png" -p rsrc_raw/pal/plans.pal --notrans -r 32

# ./datsnip tenma_02.iso 0x5CED800 0x3000 "rsrc_raw/grp/omake.bin"
# ./grpdmp_pce "rsrc_raw/grp/omake.bin" "rsrc/grp/orig/omake.png" -p rsrc_raw/pal/omake.pal --notrans -r 32

./datsnip tenma_02.iso 0x31ACB00 0x100 "rsrc_raw/grp/stonemason_bonus.bin"
#./datsnip tenma_02.iso 0x6F76300 0x100 "rsrc_raw/grp/stonemason_bonus.bin"
./spritedmp_pce "rsrc_raw/grp/stonemason_bonus.bin" "rsrc/grp/orig/stonemason_bonus.png" -n 2 -r 2
