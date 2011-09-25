require 'rexml/document'
include REXML

class Settings

    def get_state()
        root = get_settings_root()
        root.elements["State"].text
    end

    def set_state(state)
        root = get_settings_root()
        root.elements["State"].text = state
        File.open("../settings.xml", "w") { |f| f.write(root) }
    end


    private

    def get_settings_root
        file = File.new("../settings.xml")
        doc = Document.new(file)
        doc.root
    end

end