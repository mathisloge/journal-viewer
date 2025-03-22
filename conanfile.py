from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class journalViewerRecipe(ConanFile):
    name = "journal-viewer"
    version = "1.0"
    package_type = "application"

    # Optional metadata
    license = "<Put the package license here>"
    author = "mathisloge@tuta.io"
    url = "https://github.com/mathisloge/journal-viewer"
    description = "<Description of foo package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("sdl/3.2.6")
        self.requires("quill/8.2.0")
        self.requires("imgui/1.91.8-docking")
        self.requires("libsystemd/255.10")
        self.requires("fmt/11.1.4")
        self.requires("cli11/2.5.0")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    

    
