#!/bin/bash

echo 'fixing brew'

sed -i '' 's!  safe_system curl, \*args!\
  args << "--connect-timeout"\
  args << "30"\
  args << "--retry-delay"\
  args << "5"\
  args << "--retry"\
  args << "10"\
  args << "--verbose"\
  safe_system curl, *args\
!' /usr/local/Library/Homebrew/utils.rb
