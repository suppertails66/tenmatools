
set -o errexit

function doConv() {
  name=$1
  
  soffice --headless --convert-to xlsx "${name}.ods"
  rm -rf xlsx_fix
  7z x -oxlsx_fix "${name}.xlsx"
  #rm -f pom_script.xlsx
  cd xlsx_fix
    ../../xlsxfix "xl/sharedStrings.xml" "xl/sharedStrings.xml"
  #  rm -f converted.xlsx
  #  7z a -tzip converted.xlsx *
    7z a -tzip "../${name}.xlsx" *
  cd ..
  #mv xlsx_fix/converted.xlsx pom_script.xlsx
  #cp pom_script.xlsx pom_script_bak.xlsx
  ../zipbodge "${name}.xlsx"
  rm -rf xlsx_fix
}

cd script

cd ..
  make xlsxfix
  gcc src/stupid/zipbodge.c -ozipbodge
cd script

doConv "script_battle"
doConv "script_boss"
doConv "script_bossalt"
doConv "script_credits"
doConv "script_dialogue"
doConv "script_enemy"
doConv "script_generic_text"
doConv "script_info"
doConv "script_intro_ending"
doConv "script_items"
doConv "script_menu"
doConv "script_misc"
doConv "script_saveload"
doConv "script_shop"
doConv "script_subtitle"

#   soffice --headless --convert-to xlsx pom_script.ods
#   rm -r xlsx_fix
#   7z x -oxlsx_fix pom_script.xlsx
#   #rm -f pom_script.xlsx
#   cd xlsx_fix
#     ../../xlsxfix "xl/sharedStrings.xml" "xl/sharedStrings.xml"
#   #  rm -f converted.xlsx
#   #  7z a -tzip converted.xlsx *
#     7z a -tzip ../pom_script.xlsx *
#   cd ..
#   #mv xlsx_fix/converted.xlsx pom_script.xlsx
#   #cp pom_script.xlsx pom_script_bak.xlsx
#   ../zipbodge "pom_script.xlsx"
