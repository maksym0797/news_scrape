#include <libpq-fe.h>
#include <iostream>
#include <map>
#include <chrono>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <boost/uuid/uuid.hpp>             // For UUID
#include <boost/uuid/string_generator.hpp> // For string_generator
#include <boost/uuid/uuid_serialize.hpp>   // For string_generator
#include <boost/uuid/uuid_io.hpp>          // For to_string
#include <future>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <algorithm>
#include <nlohmann/json.hpp>
#include "reader/input_reader.h"
#include "config/config.h"
#include "http/http_client.h"
#include "db/postgres_client.h"

struct OllamaMessage
{
    std::string role;
    std::string content;
};

struct OllamaRequest
{
    std::string model;
    bool stream;
    OllamaMessage messages[1];
};

int testDummyParse(HttpClient webClient, int i, std::string id)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    printf("%d Current time: %s\n", i, std::ctime(&now_time));

    nlohmann::json result = webClient.get("/api/raw_post/id/" + id);

    printf("result message: %s\n", result.at("data").at("content").get<std::string>().c_str());

    return 0;
}

void testOllamaRequest(HttpClient webClient)
{
    nlohmann::json ex1 = nlohmann::json::parse(R"(
  {
    "model": "llama3.2-summarizer",
    "stream": false,
    "prompt": "Technical Aspects in Brief Here’s a closer look under the hood of Llama 3: Model Variations: Choose between 8B and 70B parameter sizes, both available in pre-trained and instruction-tuned flavors.Focus on Helpfulness and Safety: Meta prioritized helpfulness and safety during development, employing techniques like supervised fine-tuning and human feedback.Massive Training Data: Llama 3 devours text data from publicly available sources, a colossal 15 trillion tokens strong.Transformer Architecture: The core engine is a decoder-only transformer architecture, optimized for efficiency with a streamlined tokenizer.GQA for Scalability: Grouped-Query Attention (GQA) is incorporated to enhance inference efficiency.Models of Llama 3 and the Instruct ModelMeta Llama 3 boasts a diverse range of models catering to different user needs. Here’s a breakdown of the key variations:Pre-trained Models (Llama 3 Base): These versatile workhorses are ideal for general-purpose natural language generation tasks. Available in 8B and 70B parameter sizes, they excel at tasks like text summarization, question answering, and creative writing prompts.Instruction-tuned Models (Llama 3 Instruct): Optimized for human-like interaction, these models shine in dialogue applications. They are fine-tuned on massive datasets of text and code alongside explicit instructions, allowing them to follow directions, answer your questions in an informative way, and even complete tasks like writing different kinds of creative content based on your instructions. The Instruct model is currently available in the 8B and 70B parameter size.Every model can be downloaded from HuggingFace!Leveraging the Power of InstructThe Instruct model within Llama 3 offers several advantages:Improved Task Completion: It excels at understanding and completing tasks as instructed, making it ideal for applications like writing different kinds of creative text formats, translating languages, or writing different kinds of creative content.Enhanced Dialogue Experience: Instruct models foster more natural and informative conversations. They can answer your questions in a comprehensive way, follow your instructions during the conversation, and adapt their responses based on the context provided.Choosing the Right ModelThe best Llama 3 model for you depends on your specific needs. If you require a versatile tool for various text generation tasks, the pre-trained models are a great choice. For applications that involve interactive dialogue and task completion, the Instruct model is your champion.Performance BenchmarksLlama 3 reigns supreme across various industry benchmarks, surpassing its predecessors in tasks like reasoning and code generation. Check out the detailed benchmark results for a comprehensive picture about the Llama 3 Instruct models…… and the Llama 3 base models.Intended Use CasesLlama 3 caters to both commercial and research applications, with a focus on the English language.Pre-trained Models (Llama 3 Base):Text Summarization: Condense lengthy documents or articles into concise, informative summaries.Question Answering: Extract key information from text data to answer your questions in a comprehensive manner.Creative Text Generation: Spark your creativity with story prompts, poem composition, or scriptwriting assistance.Code Generation: Generate basic code snippets or translate natural language instructions into functional code (limited to simpler tasks in current versions).Data Analysis and Insights: Identify patterns and trends in textual data to support data analysis efforts.Machine Translation (future iterations): While not a primary function in the current release, future iterations of pre-trained models hold promise for basic machine translation capabilities.Instruction-tuned Models (Llama 3 Instruct):Conversational AI Assistants: Develop chatbots and virtual assistants that can engage in natural, informative dialogue, answer your questions, and complete simple tasks as instructed.Educational Tools: Craft personalized learning experiences by generating tailored practice problems, explanations, or summaries of complex concepts.Customer Service Applications: Revolutionize customer service interactions with AI-powered chatbots that can address inquiries, troubleshoot problems, and provide relevant information based on user instructions.Creative Writing Support: Break through writer’s block or explore new creative avenues with AI assistance. Instruct models can help you brainstorm ideas, generate different creative text formats of varying lengths, or provide different writing styles for your content.Code Completion and Assistance: Boost your coding productivity with AI that can suggest code completions, translate natural language instructions into code, or help debug existing code (limited to simpler tasks in current versions).Multimodal Content Creation (future iterations): As Llama 3 evolves towards multimodality, Instruct models will be instrumental in generating creative text descriptions for images, translating between different creative mediums (text to code, text to image captions), or even composing musical pieces based on textual instructions.Hardware and Software UsedUnleashing the power of a massive LLM like Llama 3 requires a robust infrastructure. Here’s a breakdown of the hardware and software that fueled its creation:Meta’s Research SuperCluster: Imagine a colossal network of powerful computers working in unison. This is Meta’s Research SuperCluster, a custom-built behemoth specifically designed for training cutting-edge AI models like Llama 3. Its immense processing power allows Llama 3 to devour massive datasets and learn complex patterns within the data.Custom Training Libraries: Meta’s team of engineers didn’t rely on off-the-shelf solutions. They developed specialized training libraries like torchtune specifically optimized for the unique architecture and training requirements of Llama 3. These custom libraries ensure efficient training, maximizing the utilization of the SuperCluster’s resources.Third-Party Cloud Compute Resources: While the SuperCluster played a crucial role in pre-training Llama 3, additional computing power was needed for fine-tuning, evaluation, and data annotation. Meta leveraged the scalability and flexibility of third-party cloud computing platforms to address these needs.Sustainability in Focus: Offsetting the Carbon FootprintTraining a massive LLM like Llama 3 requires significant computational resources, which can translate to a substantial carbon footprint. Meta prioritizes sustainability and takes a responsible approach to AI development. To address the carbon footprint associated with pre-training Llama 3, Meta’s sustainability program offsets the emissions through various initiatives.This commitment ensures that the advancements in AI brought about by Llama 3 are achieved while minimizing the environmental impact.Responsible AI DevelopmentMeta champions transparency and responsible AI development:Safety Best Practices: A series of steps were implemented to mitigate misuse and promote responsible deployment.Open Source for Faster Innovation: Open-sourcing Llama 3 fosters collaboration and accelerates advancements in safety and capabilities.Community Resources: Meta provides tools like Meta Llama Guard 2 and Code Shield to bolster safety measures in LLM applications.Beyond the Basics: A Glimpse into Llama 3’s FutureThe journey with Llama 3 doesn’t end here. Meta is actively pushing the boundaries, exploring exciting advancements that will further elevate this groundbreaking LLM:1. Multimodality: Unveiling a World Beyond TextWhile current LLMs primarily focus on text, Llama 3 aspires to break free. Future iterations will be equipped to process and generate not only text, but also data in various formats like images, audio, and potentially even video. Imagine composing a musical piece based on a written description or generating a poem inspired by a painting. Multimodality opens a treasure trove of innovative applications across creative endeavors, education, and human-computer interaction.2. Embracing Multilinguality: Breaking Down Language BarriersThe current iteration of Llama 3 operates primarily in English. However, Meta recognizes the importance of global communication. Future versions will be multilingual, trained on massive datasets of text and code in various languages. This will empower Llama 3 to:Translate languages: Seamlessly convert text from one language to another.Generate text in different languages: Craft content tailored to specific audiences.Bridge the communication gap: Enable a world where language barriers no longer exist.3. Continuous Improvement: A Commitment to ExcellenceMeta is dedicated to refining Llama 3’s capabilities through ongoing research and development. Here are some key areas of focus:Extended Context Window: Current versions process a specific context window. Future iterations will handle a larger context of information, leading to more nuanced and insightful outputs.Stronger Overall Performance: The research team is constantly seeking ways to improve Llama 3’s performance across various tasks. This might involve exploring new learning algorithms, hardware advancements, and potentially incorporating human feedback mechanisms.Transparency and Explainability: While LLMs are powerful, understanding their reasoning process is crucial. Meta strives to develop techniques that enhance transparency and explainability, allowing users to gain insights into how Llama 3 arrives at its outputs.The Open-Source Advantage: A Catalyst for ChangeMeta believes open-sourcing Llama 3 fosters a collaborative environment that fuels innovation and responsible development:Faster Innovation: Open access allows researchers and developers worldwide to contribute to Llama 3’s improvement, accelerating the pace of innovation.Enhanced Safety and Security: Open-sourcing facilitates community-driven analysis to identify and mitigate potential risks associated with LLMs.A Thriving AI Ecosystem: Open access fosters competition and collaboration, leading to better LLMs, innovative applications, and ultimately, a wider range of benefits for society.Llama 3: A Stepping Stone to a Brighter FutureMeta Llama 3 marks a significant leap forward in open-source LLMs. Its exceptional performance, diverse range of applications, and commitment to responsible development make it a valuable tool for researchers, developers, and businesses alike. As Meta continues to refine Llama 3 with advancements in multimodality, multilingual capabilities, and overall performance, we can expect even more groundbreaking applications to emerge. By fostering collaboration and open-source development, Meta hopes to shape a future where AI empowers creativity, communication, and problem-solving on a global scale. The potential unleashed by Llama 3 is vast, and the journey… has just begun!"
    }
)");
    nlohmann::json result = webClient.post("/api/generate", ex1);

    std::string response = result.at("response").get<std::string>();

    std::cout << response << std::endl;
}

int main()
{
    Config config("/Users/Maksym_Kostynskyi/Code/playground/news_scrape/cpp_scrape_consumer/.env");
    config.init();
    std::cout << "DB_CONNINFO: " << config.get("DB_CONNINFO") << std::endl;

    PostgresClient client(config);

    InputReaderDB reader(client.getDbConnection());

    HttpClient webClient(config);

    testOllamaRequest(webClient);

    std::vector<InputPost> posts = reader.read();
    std::string source_ids;

    std::cout << "Hello, World!" << std::endl;

    for (auto &post : posts)
    {
        source_ids += "\"" + boost::uuids::to_string(post.SourceID) + "\",";
    }

    std::cout << source_ids.substr(0, source_ids.size() - 1) << std::endl;

    std::vector<Source> sources = reader.getSourcesByIds(source_ids.substr(0, source_ids.size() - 1));
    std::unordered_map<boost::uuids::uuid, Source> post_source_map;
    for (auto &post : posts)
    {
        auto it = std::find_if(sources.begin(), sources.end(), [&post](const Source &source)
                               { return source.ID == post.SourceID; });

        if (it != sources.end())
        {
            post_source_map[post.ID] = *it;
        }
    }
    std::vector<std::future<int>> parsingTasks;
    for (int i = 0; i < posts.size();)
    {
        parsingTasks.clear();
        std::cout << "size: " << std::min(5, (int)(posts.size() - i)) << " " << (int)(posts.size() - i) << std::endl;
        for (int j = 0; j < std::min(5, (int)(posts.size() - i)); j++, i++)
        {
            // printf("Post ID: %d Name: %s\n", i, (posts[i].Title).c_str());
            std::cout << "Post ID: " << boost::uuids::to_string(posts[i].ID) << std::endl;

            // parsingTasks.push_back(std::async(std::launch::async, testDummyParse, webClient, i, boost::uuids::to_string(posts[i].ID)));
        }
        for (auto &task : parsingTasks)
        {
            task.wait();
        }
    }

    return 0;
}
