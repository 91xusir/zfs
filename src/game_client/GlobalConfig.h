#pragma once
#include <string>
#include <yaml-cpp/yaml.h>
#include <core/rtc2_text_ini.h>
#include <stdexcept>

/*
add by lyy2024.8.8
原来的配置类不方便，因此自己实现一个
暂时支持ini和yaml 读取值的API相同 但都只支持键值对方式
ini由原来的core提供，所以依赖core的包管理器，不能单独使用
yaml是后面给服务端准备的，可以直接使用
使用示例:

GlobalConfig configa;
configa.openFile("config.ini")//configa.openFile("config.yaml");
configa["section"]["item"]=xxx;

*/
class GlobalConfig {
   public:
    GlobalConfig() : fileType(FileType::NONE) {}

    ~GlobalConfig() { Clear(); }

    bool OpenFile(const std::string& filename);
    void SaveFile(const std::string& filename);

    // INI 文件的 SectionGetter 和 ItemGetter
    class SectionGetter {
       public:
        SectionGetter(RtIni& ini, const std::string& section)
            : m_ini(&ini), m_node(nullptr), m_section(section) {}

        SectionGetter(YAML::Node& node, const std::string& section)
            : m_ini(nullptr), m_node(&node), m_section(section) {}

        class ItemGetter {
           public:
            ItemGetter(RtIni& ini, const std::string& section, const std::string& item)
                : m_ini(&ini), m_node(nullptr), m_section(section), m_item(item) {}

            ItemGetter(YAML::Node& node, const std::string& section, const std::string& item)
                : m_ini(nullptr), m_node(&node), m_section(section), m_item(item) {}

            template <typename T>
            T at() const {
                T value;
                if (!GetValue(value)) {
                    throw std::runtime_error("Failed to get entry value.");
                }
                return value;
            }

            // 重载赋值运算符
            template <typename T>
            ItemGetter& operator=(const T& value) {
                SetValue(value);
                return *this;
            }

           private:
            // 写入值
            template <typename T>
            void SetValue(const T& value) {
                if (m_ini) {
                    if constexpr (std::is_same_v<T, std::string>) {

                        m_ini->SetEntry(m_section.c_str(), m_item.c_str(), value.c_str());
                    } else if constexpr (std::is_same_v<T, int>) {
                        m_ini->SetEntry(m_section.c_str(), m_item.c_str(), value);
                    } else if constexpr (std::is_same_v<T, bool>) {  //似乎会出错
                        m_ini->SetEntry(m_section.c_str(), m_item.c_str(), value);
                    } else if constexpr (std::is_same_v<T, long>) {
                        m_ini->SetEntry(m_section.c_str(), m_item.c_str(), value);
                    } else if constexpr (std::is_same_v<T, float>) {
                        m_ini->SetEntry(m_section.c_str(), m_item.c_str(), value);
                    } else if constexpr (std::is_same_v<T, double>) {
                        m_ini->SetEntry(m_section.c_str(), m_item.c_str(), value);
                    }
                } else if (m_node) {
                    (*m_node)[m_section][m_item] = value;
                } else {
                    throw std::runtime_error("Neither INI nor YAML node is set.");
                }
            }

            template <typename T>
            bool GetValue(T& value) const {
                if (m_ini) {
                    if constexpr (std::is_same_v<T, std::string>) {
                        const char* strValue = m_ini->GetEntry(m_section.c_str(), m_item.c_str());
                        if (strValue) {
                            value = std::string(strValue);
                            return true;
                        }
                    } else if constexpr (std::is_same_v<T, int>) {
                        return m_ini->GetEntry(m_section.c_str(), m_item.c_str(), &value);
                    } else if constexpr (std::is_same_v<T, bool>) {
                        return m_ini->GetEntry(m_section.c_str(), m_item.c_str(), &value);
                    } else if constexpr (std::is_same_v<T, long>) {
                        return m_ini->GetEntry(m_section.c_str(), m_item.c_str(), &value);
                    } else if constexpr (std::is_same_v<T, float>) {
                        return m_ini->GetEntry(m_section.c_str(), m_item.c_str(), &value);
                    } else if constexpr (std::is_same_v<T, double>) {
                        return m_ini->GetEntry(m_section.c_str(), m_item.c_str(), &value);
                    }
                } else if (m_node) {
                    if constexpr (std::is_same_v<T, std::string>) {
                        value = (*m_node)[m_section][m_item].as<std::string>();
                        return true;
                    } else if constexpr (std::is_same_v<T, int>) {
                        value = (*m_node)[m_section][m_item].as<int>();
                        return true;
                    } else if constexpr (std::is_same_v<T, bool>) {
                        value = (*m_node)[m_section][m_item].as<bool>();
                        return true;
                    } else if constexpr (std::is_same_v<T, long>) {
                        value = (*m_node)[m_section][m_item].as<long>();
                        return true;
                    } else if constexpr (std::is_same_v<T, float>) {
                        value = (*m_node)[m_section][m_item].as<float>();
                        return true;
                    } else if constexpr (std::is_same_v<T, double>) {
                        value = (*m_node)[m_section][m_item].as<double>();
                        return true;
                    }
                }
                return false;
            }

            RtIni*      m_ini;
            YAML::Node* m_node;
            std::string m_section;
            std::string m_item;
        };

        ItemGetter operator[](const std::string& item) const {
            if (m_ini) {
                return ItemGetter(*m_ini, m_section, item);
            } else if (m_node) {
                return ItemGetter(*m_node, m_section, item);
            }
            throw std::runtime_error("Neither INI nor YAML node is set.");
        }

        ItemGetter operator[](const std::string& item) {
            if (m_ini) {
                return ItemGetter(*m_ini, m_section, item);
            } else if (m_node) {
                return ItemGetter(*m_node, m_section, item);
            }
            throw std::runtime_error("Neither INI nor YAML node is set.");
        }

       private:
        RtIni*      m_ini;
        YAML::Node* m_node;
        std::string m_section;
    };

    SectionGetter operator[](const std::string& section) {
        if (fileType == FileType::INI) {
            return SectionGetter(rtIni, section);
        } else if (fileType == FileType::YAML) {
            return SectionGetter(rootNode, section);
        }
        throw std::runtime_error("File type not supported.");
    }

   private:
    void Clear();
    enum class FileType { NONE, YAML, INI };
    YAML::Node rootNode;
    RtIni      rtIni;
    FileType   fileType = FileType::NONE;
};
