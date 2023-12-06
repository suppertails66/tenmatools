
set -o errexit

tempFontFile=".fontrender_temp"


function outlineSolidPixels() {
#  convert "$1" \( +clone -channel A -morphology EdgeOut Diamond:1 -negate -threshold 15% -negate +channel +level-colors \#000024 \) -compose DstOver -composite "$2"
  convert "$1" \( +clone -channel A -morphology EdgeOut Square:1 -negate -threshold 15% -negate +channel +level-colors \#000024 \) -compose DstOver -composite "$2"
}

function renderString() {
  printf "$2" > $tempFontFile
  
#  ./fontrender "font/12px_outline/" "$tempFontFile" "font/12px_outline/table.tbl" "$1.png"
#  ./fontrender "font/" "$tempFontFile" "font/table.tbl" "$1.png"
  ./fontrender "font/" "$tempFontFile" "font/table.tbl" "$1.png"
}

function renderStringNarrow() {
  printf "$2" > $tempFontFile
  
#  ./fontrender "font/12px/" "$tempFontFile" "font/12px/table.tbl" "$1.png"
  ./fontrender "font/narrow/" "$tempFontFile" "font/narrow/table.tbl" "$1.png"
#   outlineSolidPixels "$1.png" "$1.png"
}

function renderStringNarrower() {
  printf "$2" > $tempFontFile
  
  ./fontrender "font/narrower/" "$tempFontFile" "font/narrower/table.tbl" "$1.png"
#   outlineSolidPixels "$1.png" "$1.png"
}

function renderStringTall() {
  printf "$2" > $tempFontFile
  
#  ./fontrender "font/12px_outline/" "$tempFontFile" "font/12px_outline/table.tbl" "$1.png"
#  ./fontrender "font/" "$tempFontFile" "font/table.tbl" "$1.png"
  ./fontrender "font/tall/" "$tempFontFile" "font/tall/table.tbl" "$1.png"
}

function renderStringScene() {
  printf "$2" > $tempFontFile
  
#  ./fontrender "font/12px_outline/" "$tempFontFile" "font/12px_outline/table.tbl" "$1.png"
#  ./fontrender "font/" "$tempFontFile" "font/table.tbl" "$1.png"
  ./fontrender "font/scene/" "$tempFontFile" "font/scene/table.tbl" "$1.png"
}

function renderStringMecosPre() {
  ./scrdat_render "font/mecos/" "font/mecos/table.tbl" "$2" "table/ascii.tbl" "$1.png" --fgcolor 255 255 255 --bgcolor 0 0 0 -w 256 -h 224
}

function renderStringReportPre() {
  ./scrdat_render "font/report/" "font/report/table.tbl" "$2" "table/ascii.tbl" "$1.png" --fgcolor 0x00 0x00 0x00 --bgcolor 0xDA 0xDA 0xB6 -w 256 -h 176
}



make blackt && make fontrender

# renderString render1 "Script"
# renderString render2 "Producer"
# renderString render3 "Music"
# renderString render4 "Programmer"
# renderString render5 "Designers"
# renderString render6 "Costumes"
# renderString render7 "Stunts"
# renderString render8 "Acting Coach"
# renderString render9 "Support"
# renderString render10 "Director"
# 
# renderString render11 "Masatsukasa Shinda"
# renderString render12 "Innocence Mikami"
# renderString render13 "Joseph Shindo"
# renderString render14 "Marronnier Fukuda"
# renderString render15 "Linda Kubo"
# renderString render16 "Jodtinktur Okada"
# renderString render17 "Drunkard (28)"
# renderString render18 "Shinichi Nakata"
# renderString render19 "Theater Troupe {Q}"
# renderStringNarrow render20 "main office"

# renderString render31 ""
# renderString render32 ""
# renderString render33 ""
# renderString render34 ""
# renderString render35 ""
# renderString render36 ""
# renderString render37 ""
# renderString render38 ""
# renderString render39 ""
# renderString render30 ""

# #renderStringTall render1 "FOLK TALE MASTERPIECES"
# renderStringTall render1 "FOLK TALE CLASSICS"

# renderString render1 "This translation is designed exclusively for"
# renderString render2 "use with the Super CD-ROM System."
# renderString render3 "If you wish to play it, please use a"
# renderString render4 "version 3.0 or higher System Card."
# renderString render5 "You may press the Run Button to play the"
# renderString render6 "game, but it will remain in its original language."

# renderString render1 "This translation is designed exclusively for"
# renderString render2 "use with the Super CD-ROM System. Please"
# renderString render3 "use a version 3.0 or higher System Card."
# renderString render4 "You may press the Run Button to play, but"
# renderString render5 "the game will remain in its original language."

#renderStringNarrower render1 "Sword of the Divine"
renderStringNarrower render1 "White Pheasant Tail"

rm -f $tempFontFile