#!/usr/bin/bash
date > ~/cronTest.txt
text=$(git status | grep add)
if [ -z "$text" ]; then
   echo "String is empty"
else
	git commit -a -m "Commited by cron at $(date +%H:%M" "%Y-%m-%d)"
	git push origin main
	git push github main
fi
