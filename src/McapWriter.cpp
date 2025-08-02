#include "McapWriter.h"
#include "BuildFileDescriptorSet.h"
#include "MyLog.h"
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <filesystem>

mcl::McapWriter::~McapWriter() {
    _writer->close();
}

mcap::ChannelId mcl::McapWriter::create_channel(    const std::string &msg_name, 
                                                    const std::string &channel_name, 
                                                    const google::protobuf::Descriptor *desc) 
{
    mcap::Schema schema(msg_name, "protobuf", foxglove::BuildFileDescriptorSet(desc).SerializeAsString());
    _writer->addSchema(schema);

    mcap::Channel channel(channel_name, "protobuf", schema.id);
    _writer->addChannel(channel);

    return channel.id;
}

void mcl::McapWriter::init_mcap_logger( const std::unordered_map<uint32_t,mydesc_t>& desc_map,  
                                        const std::string& socket_name,
                                        const std::string& mcap_file_name) 
{
    std::filesystem::path file_path(mcap_file_name);
    std::filesystem::path dir_path = file_path.parent_path();

    if (!dir_path.empty() && !std::filesystem::exists(dir_path)) {
        try {
            std::filesystem::create_directories(dir_path);
        } catch (const std::filesystem::filesystem_error& e) {
            mcl::log_error("Failed to create directories for mcap log file",
                           {{"Error", e.what()}, {"Directory", dir_path.string()}});
            std::exit(EXIT_FAILURE);
        }
    }

    if(!_writer->open(mcap_file_name, mcap::McapWriterOptions("")).ok()){
        mcl::log_error( "Failed to create mcap log file",
                        {{"Filename", mcap_file_name}});
        std::exit(EXIT_FAILURE);
    }

    for (const auto&[id, desc_map] : desc_map) {
        auto it = desc_map.find(-1);
        if(it == desc_map.end()) {
            for (const auto& [mux_val, desc] : desc_map) {
                std::string channel_name = std::string(socket_name + "/" + desc->name());
                mcap::ChannelId channel_id = create_channel(    desc->name(),
                                                                channel_name,
                                                                desc);
                _name_channel_map.emplace(desc->name(), std::move(channel_id));
            }
        } else {
            const google::protobuf::Descriptor* desc;
            if(desc_map.count(-1)) {
                desc = desc_map.at(-1);
            } else {
                mcl::log_error( "No Mux Message contains nothing at position -1",
                                {{"ID", std::to_string(id)}});
                std::exit(EXIT_FAILURE);
            }
            std::string channel_name = std::string(socket_name + "/" + desc->name());
            mcap::ChannelId channel_id = create_channel(    desc->name(),
                                                            channel_name,
                                                            desc);
            _name_channel_map.emplace(desc->name(), std::move(channel_id));
        }
    }
}

void mcl::McapWriter::create_mcap_message(  mcap::Message &mcap_msg, 
                                            std::shared_ptr<std::string> msg_str, 
                                            mcap::ChannelId channel_id) 
{
    mcap::Timestamp startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                std::chrono::system_clock::now().time_since_epoch())
                                .count();

    mcap_msg.channelId = channel_id;
    mcap_msg.sequence = 0;
    mcap_msg.publishTime = startTime;
    mcap_msg.logTime = startTime;
    mcap_msg.data = reinterpret_cast<const std::byte*>(msg_str->data());
    mcap_msg.dataSize = msg_str->size();
}


void mcl::McapWriter::write_msg(    std::shared_ptr<std::string> msg, 
                                    const std::string &msg_name) {


    auto it = _name_channel_map.find(msg_name);

    if(it == _name_channel_map.end()) {
        mcl::log_error( "Cant find channel id",
                        {{"Messagename", msg_name}});
        std::exit(EXIT_FAILURE);
    }

    

    mcap::ChannelId channel_id = it->second;

    mcap::Message mcap_msg;

    create_mcap_message(    mcap_msg,
                            msg,
                            channel_id);

    auto ret = _writer->write(mcap_msg);
    if(!ret.ok()) {
        mcl::log_error("Error while writing");
        _writer->terminate();
        std::exit(-1);
    }

}