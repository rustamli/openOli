class Submit
  attr_accessor :message, :id, :num_tests
  
  def initialize
    @id = ARGV[0]
    @problem_id = ARGV[1]
    @lang = ARGV[2]
    @cur_test = 0
    @message = ""
  end

  def compile

    if (@lang!="java")
       system "sh compilers/#{@lang}.sh submits/#{@id}/program.#{@lang} submits/#{@id}/program 2> submits/#{@id}/compile.err 1> submits/#{@id}/compile.inf"    
    else
       system "sh compilers/java.sh submits/#{@id}/Main.java submits/#{@id} 2> submits/#{@id}/compile.err 1> submits/#{@id}/compile.inf"
    end

    error = $?.exitstatus
    @message = "Compilation Error" if error == 1
  end

  def get_settings
    file = File.new("problems/#{@problem_id}/settings.xml")
    doc = Document.new(file)
    @root = doc.root
    @timelimit = @root.elements["limits"].attributes["timelimit"].to_i
    @memorylimit = @root.elements["limits"].attributes["memorylimit"].to_i
    @outputlimit = @root.elements["limits"].attributes["outputlimit"].to_i
    @detalisation = @root.elements["limits"].attributes["detalisation"].to_i
    @num_tests = @root.elements["tests"].attributes["num"].to_i
    @cur_test = 0
  end

  def next_test
    @cur_test += 1
  end

  def run_program
   
    lang_code = case @lang
      when "c" then 0
      when "cpp" then 1
      when "pas" then 2
      when "java" then 3
    end

    system "ruby sudorun.rb #{@timelimit} #{@memorylimit} #{@outputlimit} #{@problem_id} #{@id} #{@cur_test} #{lang_code} <secret.txt"

   result_file = File.open("submits/#{@id}/runinfo.txt")
   result_line = result_file.readline
   
   result = result_line.split(" ")

   @message = case result[0].to_i
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
    @agent = @root.elements["tests/test[@id=#{@cur_test.to_s}]"].attributes['agent']

    if @agent == "singleout"
      system "ruby agents/compare.rb #{@detalisation} problems/#{@problem_id}/out_#{@cur_test}.txt submits/#{@id}/stdout.txt"
      es = $?.exitstatus
      @message="Wrong Answer" if es == 1
    elsif @agent == "multiout"
      outnum = @root.elements["tests/test[@id=#{@cur_test.to_s}]"].attributes['outnum'].to_i
      
      i = 1
      es = 1
      while ((i <= outnum)&&(es != 0))
        system "ruby agents/compare.rb #{@detalisation} problems/#{@problem_id}/out_#{@cur_test}_#{i}.txt submits/#{@id}/stdout.txt"
        es = $?.exitstatus
        i += 1
      end
      @message="Wrong Answer" if es == 1
    else
      system "./agents/#{@agent} < submits/#{@id}/stdout.txt"
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
   tests = @cur_test-1
   tests = @cur_test if @outcome == "accepted"
   tests = 0 if tests == -1
    File.open("submits/#{@id}/report.xml", 'w+') {|f| 
      @outcome = "failed" if @outcome != "accepted"
      f.write("<?xml version=\"1.0\"?>\n")
      f.write("<result tests=\"#{tests}\" outcome=\"#{@outcome}\">#{@message}</result>\n")
    }
    exit
  end

end

=begin
 * Name: submit.rb
 * class Submit
 * Author: Turan Rustamli
 * Date: 27.02.2010 23:47
 * License: MIT
=end
