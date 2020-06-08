#include <fstream>
#include <sstream>
#include <string>
#include "Graphics/Modeling/WavefrontMaterial.h"

namespace GRAPHICS::MODELING
{
    /// Attempts to load the material from the specified .mtl file.
    /// @param[in]  mtl_filepath - The path of the .mtl file to load.
    /// @return The material, if successfull loaded; null otherwise.
    std::shared_ptr<Material> WavefrontMaterial::Load(const std::filesystem::path& mtl_filepath)
    {
        // OPEN THE FILE.
        std::ifstream material_file(mtl_filepath);
        bool material_file_opened = material_file.is_open();
        if (!material_file_opened)
        {
            return nullptr;
        }

        // READ IN THE DATA FROM THE .OBJ FILE.
        // Note that this reading may not yet be fully robust.
        // It only handles the absolute minimum as currently needed for basic demos.
        constexpr char SPACE_SEPARATOR = ' ';
        /// @todo   Handle multiple materials?
        auto material = std::make_shared<Material>();
        /// @todo   Handle different shading models?
        material->Shading = ShadingType::MATERIAL;
        std::string line;
        while (std::getline(material_file, line))
        {
            // SKIP OVER ANY BLANK LINES.
            bool is_blank_line = line.empty();
            if (is_blank_line)
            {
                continue;
            }

            // SKIP OVER ANY COMMENT LINES.
            constexpr char MTL_COMMENT_CHARACTER = '#';
            bool is_comment_line = line.starts_with(MTL_COMMENT_CHARACTER);
            if (is_comment_line)
            {
                continue;
            }

            // CHECK IF A NEW MATERIAL IS BEING DEFINED.
            /// @todo   Should we track this?
            const std::string NEW_MATERIAL_KEYWORD = "newmtl";
            bool is_new_material_line = line.starts_with(NEW_MATERIAL_KEYWORD);
            if (is_new_material_line)
            {
                continue;
            }

            // READ IN ANY SPECULAR EXPONENT.
            const std::string SPECULAR_EXPONENT_INDICATOR = "Ns";
            bool is_specular_exponent_line = line.starts_with(SPECULAR_EXPONENT_INDICATOR);
            if (is_specular_exponent_line)
            {
                /// @todo   Make this more efficient.
                std::istringstream line_data(line);
                // Skip past the data type indicator.
                std::string data_type_indicator;
                line_data >> data_type_indicator;

                line_data >> material->SpecularPower;

                continue;
            }

            // READ IN ANY AMBIENT COLOR.
            const std::string AMBIENT_COLOR_INDICATOR = "Ka";
            bool is_ambient_color_line = line.starts_with(AMBIENT_COLOR_INDICATOR);
            if (is_ambient_color_line)
            {
                /// @todo   Make this more efficient.
                std::istringstream line_data(line);
                // Skip past the color type indicator.
                std::string color_type_indicator;
                line_data >> color_type_indicator;

                Color color = Color::BLACK;
                line_data >> color.Red;
                line_data >> color.Green;
                line_data >> color.Blue;

                material->AmbientColor = color;

                continue;
            }

            // READ IN ANY DIFFUSE COLOR.
            const std::string DIFFUSE_COLOR_INDICATOR = "Kd";
            bool is_diffuse_color_line = line.starts_with(DIFFUSE_COLOR_INDICATOR);
            if (is_diffuse_color_line)
            {
                /// @todo   Make this more efficient.
                std::istringstream line_data(line);
                // Skip past the color type indicator.
                std::string color_type_indicator;
                line_data >> color_type_indicator;

                Color color = Color::BLACK;
                line_data >> color.Red;
                line_data >> color.Green;
                line_data >> color.Blue;

                material->DiffuseColor = color;
#if 0
                /// @todo   Using this for the material color is just a hack for now.
                /// Adding duplicates to handle triangles for some cases.
                material->WireframeColor = color;
                material->FaceColor = color;
                material->VertexFaceColors.push_back(color);
                material->VertexFaceColors.push_back(color);
                material->VertexFaceColors.push_back(color);
                material->VertexWireframeColors.push_back(color);
                material->VertexWireframeColors.push_back(color);
                material->VertexWireframeColors.push_back(color);
#endif
                
                continue;
            }

            // READ IN ANY SPECULAR COLOR.
            const std::string SPECULAR_COLOR_INDICATOR = "Ks";
            bool is_specular_color_line = line.starts_with(SPECULAR_COLOR_INDICATOR);
            if (is_specular_color_line)
            {
                /// @todo   Make this more efficient.
                std::istringstream line_data(line);
                // Skip past the color type indicator.
                std::string color_type_indicator;
                line_data >> color_type_indicator;

                Color color = Color::BLACK;
                line_data >> color.Red;
                line_data >> color.Green;
                line_data >> color.Blue;

                material->SpecularColor = color;

                continue;
            }

            // READ IN ANY EMISSIVE COLOR.
            const std::string EMISSIVE_COLOR_INDICATOR = "Ke";
            bool is_emissive_color_line = line.starts_with(EMISSIVE_COLOR_INDICATOR);
            if (is_emissive_color_line)
            {
                /// @todo   Make this more efficient.
                std::istringstream line_data(line);
                // Skip past the color type indicator.
                std::string color_type_indicator;
                line_data >> color_type_indicator;

                Color color = Color::BLACK;
                line_data >> color.Red;
                line_data >> color.Green;
                line_data >> color.Blue;

                material->EmissiveColor = color;

                continue;
            }

            /// @todo   Not sure what "Ni" means.
            /// https://en.wikipedia.org/wiki/Wavefront_.obj_file#Material_template_library
            /// says optical density (index of refraction).
            
            // READ IN ANY OPAQUENESS LEVEL.
            constexpr char DISSOLVED_LEVEL_INDICATOR = 'd';
            bool is_dissolved_level_line = line.starts_with(DISSOLVED_LEVEL_INDICATOR);
            if (is_dissolved_level_line)
            {
                /// @todo   Make this more efficient.
                std::istringstream line_data(line);
                // Skip past the data type indicator.
                std::string data_type_indicator;
                line_data >> data_type_indicator;

                // UPDATE THE ALPHA FOR ALL COLORS.
                float alpha = Color::MAX_FLOAT_COLOR_COMPONENT;
                line_data >> alpha;
                /// @todo   Using this for the material color is just a hack for now.
                /// Adding duplicates to handle triangles for some cases.
                material->WireframeColor.Alpha = alpha;
                material->FaceColor.Alpha = alpha;
                for (auto& color : material->VertexFaceColors)
                {
                    color.Alpha = alpha;
                }
                for (auto& color : material->VertexWireframeColors)
                {
                    color.Alpha = alpha;
                }

                material->AmbientColor.Alpha = alpha;
                material->DiffuseColor.Alpha = alpha;
                material->SpecularColor.Alpha = alpha;
                material->EmissiveColor.Alpha = alpha;

                continue;
            }

            // READ IN THE ILLUMINATION MODEL.
            const std::string ILLUMINATION_MODEL_INDICATOR = "illum";
            bool is_illumination_model_line = line.starts_with(ILLUMINATION_MODEL_INDICATOR);
            if (is_illumination_model_line)
            {
                /// @todo
                continue;
            }
        }

        // RETURN THE MATERIAL.
        return material;
    }
}
