set -o errexit

#soffice --headless --convert-to ods --infilter=CSV:44,34,64 $file

#mkdir -p test
#soffice --headless --convert-to csv --infilter=CSV:44,34,64 "yuna_script.ods" 
#soffice --headless --convert-to csv:"Text - txt - csv (StarCalc)" "yuna_script.ods"
#soffice --headless --convert-to xlsx --outdir test yuna_script.ods

cd script
#soffice --headless --convert-to csv --infilter=CSV:44,34,76 --outdir test yuna_script.ods
#soffice --headless --convert-to csv --infilter=CSV:44,34,76 script.xlsx

soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_battle.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_boss.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_bossalt.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_credits.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_dialogue.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_enemy.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_generic_text.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_info.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_intro_ending.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_items.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_menu.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_misc.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_saveload.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_shop.ods
soffice --headless --convert-to csv --infilter=CSV:44,34,76 script_subtitle.ods
