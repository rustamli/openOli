# openOli Default Checker

class DefaultChecker
    
    def check
        submit = Submit.new

        submit.compile
        submit.report if submit.message!=""

        submit.get_settings

        submit.num_tests.times do
          submit.next_test
          submit.run_program
          submit.report if submit.message!=""

          submit.analyse_out
          submit.report if submit.message!=""
        end

        submit.accept
        submit.report
    end
end
