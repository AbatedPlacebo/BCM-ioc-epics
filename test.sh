#!/usr/bin/bash
date > ~/cronTest.txt
text=$(git status | grep add)
if [ -z "$text" ]; then
   echo "String is empty"
else
	git add :/
	git commit -m "Commited by cron at $(date +%H:%M" "%Y-%m-%d)"
	git push origin main
fi
