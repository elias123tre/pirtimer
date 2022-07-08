#!/bin/bash

set -e

cd ~/pirtimer

COMP=g++
ARGS="-Wall -Wextra -Werror -Wfatal-errors -pthread -fconcepts"

FILENAME=$2
BASENAME="${FILENAME%%.*}"

if [ "$1" = "build" ]; then
  if [ "$2" = "" ]; then
    echo "Building..."
    $COMP ${PWD##*/}.cpp $ARGS
    echo "Restarting..."
    sudo systemctl restart pirtimer
    echo "Done!"
  else
    echo "Building..."
    $COMP $2 $ARGS -o $BASENAME.out
    echo "Done!"
  fi
elif [ "$1" = "test" ]; then
  echo "Building..."
  $COMP $2 $ARGS -o $BASENAME.out
  echo "Running..."
  ./$BASENAME.out
  echo "Cleaning..."
  rm ./$BASENAME.out
  echo "Done!"
fi
