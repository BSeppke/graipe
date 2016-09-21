#!/bin/bash

echo "Generating bundle for deployment of Graipe"

rm -rf ./Graipe.app
cp -R ../bin/Graipe.app .

for module in features images vectorfields analysis featuredetection featurematching imagefilter imageprocessing  multispectral opticalflow vectorfieldprocessing racerclient registration winddetection
do
	cp ../bin/libgraipe_$module.dylib ./Graipe.app/Contents/MacOS
	install_name_tool -id libgraipe_$module.dylib ./Graipe.app/Contents/MacOS/libgraipe_$module.dylib
done

/opt/local/libexec/qt5/bin/macdeployqt ./Graipe.app -dmg

echo "Generation done"

