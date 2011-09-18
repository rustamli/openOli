
require 'openoli/meta/settings.rb'
require 'openoli/meta/submit_info.rb'

INBOX_PATH = "../inbox"

def main
    command = ARGV[0]
    settings = Settings.new

    if (command == "start")
        puts "ON"
    elsif (command == "stop")
        puts "OFF"
    end

    #while (true) do
        submits = check_inbox_for_submits
        submits.each{ |submit| process_submit(submit) }
        #sleep 3
    #end
end

def check_inbox_for_submits
    submits = Dir.entries(INBOX_PATH)
    submits.delete(".")
    submits.delete("..")
    submits.sort[0, 6]
end

def process_submit(submit_id)
    submit_info = SubmitInfo.create_from_xml(INBOX_PATH + "/" + submit_id + "/info.xml")
    puts(submit_info.id)
    puts(submit_info.lang)
    puts(submit_info.problem_id)
end

main()

=begin
 * Name: openoli.rb
 * openOli the base script
 * Author: Turan Rustamli
 * Date: 19.09.2011 00:52
 * First Release: 27.02.2010 23:35
 * License: MIT
=end