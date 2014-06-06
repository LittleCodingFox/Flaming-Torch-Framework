echo "Please type the project name (complete from start to end, e.g. 'Codename Sul Story' without the ''s):" 
read PROJECTNAME

cd "../$PROJECTNAME"

sh ./finishbuild.sh
