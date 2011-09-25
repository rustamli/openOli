
class SubmitInfo

    attr_accessor :id, :lang, :problem_id, :checker

    def SubmitInfo.create_from_xml(xml_file_path)
        file = File.new(xml_file_path)
        doc = Document.new(file)
        root = doc.root
        
        id = root.attributes["Id"]
        lang = root.elements["Language"].text
        problem_id = root.elements["Problem"].text

        new(id, lang, problem_id)
    end

    def initialize(id, lang, problem_id)
        @id, @lang, @problem_id = id, lang, problem_id

        @checker = get_checker_by_problem_id()
    end

    private

    def get_checker_by_problem_id
        if (@problem_id[0, 2] == "acm")
            :acm_checker
        else
            :default_checker
        end
    end

end