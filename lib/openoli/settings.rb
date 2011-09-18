require 'rexml/document'
include REXML

class Settings

    def getState()
        root = getSettingsRoot()
        root.elements["State"].text
    end

    def setState(state)
        root = getSettingsRoot()
    end


    private

    def getSettingsRoot
        file = File.new("../settings.xml")
        doc = Document.new(file)
        doc.root
    end

end