
require 'openoli/meta/settings.rb'
require 'openoli/meta/submit_info.rb'
require 'openoli/submit.rb'
require 'openoli/checkers/default_checker.rb'

INBOX_PATH = "../inbox"

def main
    command = ARGV[0]
    settings = Settings.new
    
    if (command == "start")
        state = "ON"
        settings.set_state(state)

        while (state != "OFF") do
            submits = check_inbox_for_submits
            submits.each{ |submit| process_submit(submit) }
            
            sleep 3
            state = settings.get_state()
            puts state
        end
    elsif (command == "stop")
        settings.set_state("OFF")
    end
end

def check_inbox_for_submits
    submits = Dir.entries(INBOX_PATH)
    submits.delete(".")
    submits.delete("..")
    submits.sort[0, 6]
end

def process_submit(submit_id)
    submit_info = SubmitInfo.create_from_xml(INBOX_PATH + "/" + submit_id + "/info.xml")
    submit = Submit.new(submit_info)

    checker = get_checker_by_name(submit_info.checker)
    checker.check(submit)
end

def get_checker_by_name(checker_type)
    if (checker_type == :default_checker)
        DefaultChecker.new
    end
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