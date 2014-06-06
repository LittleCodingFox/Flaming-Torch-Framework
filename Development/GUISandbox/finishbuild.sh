export DESTINATION=Artwork/PackageData/Default

mkdir -p $DESTINATION

export SOURCE=../Artwork/Logo/

cp $SOURCE/torch_withtext_v3.png $DESTINATION/torch_withtext.png
cp $SOURCE/torch_small_v3.png $DESTINATION/torch_small.png

export SOURCE=../Artwork

cp $SOURCE/*.ttf $DESTINATION

cp -R $SOURCE/UIThemes $DESTINATION

export DESTINATION=../../Content

cp $1 $2

$3 $4 $5 $6

rm -Rf $5
