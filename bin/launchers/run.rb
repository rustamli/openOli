
RUN_PATH = "../bin/launchers"
PROBLEMS_PATH = "../../data/problems"
SUBMITS_PATH = "../../inbox"

submit_id = ARGV[0]
problem_id = ARGV[1]
current_test = ARGV[2]

tl = ARGV[3]
ml = ARGV[4]
ol = ARGV[5]

cmd = "cd #{RUN_PATH}\n"
cmd += "sudo ./openolir #{SUBMITS_PATH}/#{submit_id}/program #{PROBLEMS_PATH}/#{problem_id}/in_#{current_test}.txt #{SUBMITS_PATH}/#{submit_id}/stdout.txt #{tl} #{ml} #{ol} > #{SUBMITS_PATH}/#{submit_id}/runinfo.txt"

system cmd

exit $?.exitstatus

=begin
 * Name: run.rb
 * openOli Runner
 * Author: Turan Rustamli
 * Date: 27.02.2010 23:58
 * License: MIT
=end
