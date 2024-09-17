#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <unordered_map>

/**
 * @class Config
 * @brief A class to handle configuration settings.
 *
 * The Config class is responsible for managing configuration settings
 * from a specified configuration file path.
 *
 * @note The constructor is marked as explicit to prevent implicit conversions.
 *       This means that the constructor cannot be called with a single argument
 *       unless it is explicitly specified. This helps to avoid unintended
 *       conversions and potential bugs.
 *
 * @param configPath The path to the configuration file.
 */
class Config
{
public:
    explicit Config(const std::string &configPath);

    void init();

    std::string get(const std::string &key);

private:
    std::string configPath;
    std::unordered_map<std::string, std::string> configMap;
};

#endif // CONFIG_H