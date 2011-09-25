class Submit
  attr_accessor :message, :id, :num_tests
  
  COMPILERS_PATH = "../bin/compilers"
  PROBLEMS_PATH = "../data/problems"
  SUBMITS_PATH = "../inbox"
  AGENTS_PATH = "../bin/agents"
  PROGRAM_LAUNCHER = "../bin/launchers/run.rb"
  APPROVER = "sh ../bin/utils/approve.sh"

  def initialize(submit_info)
    @id = submit_info.id
    @problem_id = submit_info.problem_id
    @lang = submit_info.lang
    
    @cur_test = 1
    @message = ""
  end

  def compile

    if (@lang!="java")
       system "sh #{COMPILERS_PATH}/#{@lang}.sh #{SUBMITS_PATH}/#{@id}/source.#{@lang} #{SUBMITS_PATH}/#{@id}/program 2> #{SUBMITS_PATH}/#{@id}/compile.err 1> #{SUBMITS_PATH}/#{@id}/compile.inf"    
    else
       system "sh #{COMPILERS_PATH}/java.sh #{SUBMITS_PATH}/#{@id}/Main.java submits/#{@id} 2> #{SUBMITS_PATH}/#{@id}/compile.err 1> #{SUBMITS_PATH}/#{@id}/compile.inf"
    end

    error = $?.exitstatus
    @message = "Compilation Error" if error == 1
  end

  def get_problem_settings
    file = File.new("#{PROBLEMS_PATH}/#{@problem_id}/settings.xml")
    doc = Document.new(file)
    
    @problem_xml_root = doc.root
    @timelimit = @problem_xml_root.elements["limits"].attributes["timelimit"].to_i
    @memorylimit = @problem_xml_root.elements["limits"].attributes["memorylimit"].to_i
    @outputlimit = @problem_xml_root.elements["limits"].attributes["outputlimit"].to_i
    @detalisation = @problem_xml_root.elements["limits"].attributes["detalisation"].to_i
    @num_tests = @problem_xml_root.elements["tests"].attributes["num"].to_i
  end

  def next_test
    @cur_test += 1
  end

  def get_lang_code
    case @lang
      when "c" then 0
      when "cpp" then 1
      when "pas" then 2
      when "java" then 3
    end
  end

  def run_program
   
    lang_code = get_lang_code()

    system "ruby #{PROGRAM_LAUNCHER} #{@id} #{@problem_id} #{@cur_test} #{@timelimit} #{@memorylimit} #{@outputlimit}"

    result_file = File.open("#{SUBMITS_PATH}/#{@id}/runinfo.txt")
    result_line = result_file.readline
    result = result_line.split(" ")[0].to_i

    @message = case result
      when 3 then "Time Limit Exceeded"
      when 6 then "Output Limit Exceeded"
      when 4 then "Memory Limit Exceeded"
      when 8 then "Runtime Error (RE_SEGV)"
      when 9 then "Runtime Error (RE_FPE)"
      when 10 then "Runtime Error (RE_BUS)"
      when 11 then "Runtime Error (RE_ABRT)"
      when 12 then "Runtime Error (RE_UNKNOWN)"
      when 13 then "System Access Denied"
      when 14 then "System Error"
      else ""
    end
      
  end

  def analyse_out
    @agent = @problem_xml_root.elements["tests/test[@id=#{@cur_test.to_s}]"].attributes['agent']

    if @agent == "singleout"
      
      system "ruby #{AGENTS_PATH}/compare.rb #{@detalisation} #{PROBLEMS_PATH}/#{@problem_id}/out_#{@cur_test}.txt #{SUBMITS_PATH}/#{@id}/stdout.txt"
      es = $?.exitstatus
      @message="Wrong Answer" if es == 1

    elsif @agent == "multiout"
      outnum = @root.elements["tests/test[@id=#{@cur_test.to_s}]"].attributes['outnum'].to_i
      
      i = 1
      es = 1
      while ((i <= outnum)&&(es != 0))
        system "ruby #{AGENTS_PATH}/compare.rb #{@detalisation} #{PROBLEMS_PATH}/#{@problem_id}/out_#{@cur_test}_#{i}.txt #{SUBMITS_PATH}/#{@id}/stdout.txt"
        es = $?.exitstatus
        i += 1
      end

      @message="Wrong Answer" if es == 1

    else
      system "#{AGENTS_PATH}/#{@agent} < #{SUBMITS_PATH}/#{@id}/stdout.txt"
      es = $?.exitstatus

      @message="Wrong Answer" if es == 1
    end

  end

  def accept
    @outcome = "accepted"
    @message = "Accepted"
  end

  def report
    #International Collegiate Programming Contest Validator Interface Standard
    tests = @cur_test - 1

    File.open("#{SUBMITS_PATH}/#{@id}/report.xml", 'w+') {|f| 
      @outcome = "failed" if @outcome != "accepted"
      f.write("<?xml version=\"1.0\"?>\n")
      f.write("<result tests=\"#{tests}\" outcome=\"#{@outcome}\">#{@message}</result>\n")
    }

    system "#{APPROVER} #{SUBMITS_PATH}/#{@id} #{@id}"

  end

end

=begin
 * Name: submit.rb
 * class Submit
 * Author: Turan Rustamli
 * Date: 27.02.2010 23:47
 * License: MIT
=end
