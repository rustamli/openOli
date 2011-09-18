# openOli Main
require 'submit'
require 'rexml/document'
include REXML

#alpha
submit = Submit.new

#beta
submit.compile
submit.report if submit.message!=""


#gamma
submit.get_settings

submit.num_tests.times do
  submit.next_test
  submit.run_program
  submit.report if submit.message!=""

  #delta
  submit.analyse_out
  submit.report if submit.message!=""
end

#omega
submit.accept
submit.report

=begin
 * Name: main.rb
 * openOli the base script
 * Author: Turan Rustamli
 * Date: 27.02.2010 23:35
 * License: MIT
=end
