
class SubmitInfo

    attr_accessor :id, :lang, :problem_id

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
    end

end