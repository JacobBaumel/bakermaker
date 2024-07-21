#include "UI/UsedStorage.h"

#include <improgress.h>
#include <utils.h>
#include <UI/RepoManage.h>
#include <sstream>

#include "libssh/libssh.h"
#include "ssh_helper.h"

#define MSG_BUFFER_LENGTH 16


namespace bakermaker {
    static constexpr int FS_USED = 0;
    static constexpr int FS_AVAIL = 1;
    static constexpr int DIR_USED = 2;

    UsedStorage::UsedStorage() :
        BaseUIScreen(bakermaker::ProgramStage::USED_STORAGE, &bakermaker::configScreens), exec(nullptr),
        execDone(false), success(0), totalAvail(0), totalUsed(0), hasRefreshed(false) {
    }

    void UsedStorage::render() {
        ImGui::PushFont(fontlist[1]);
        ImGui::Text("Storage Usage");
        ImGui::PopFont();
        ImGui::Separator();

        if(!hasRefreshed) ImGui::BeginDisabled();

        ImGui::PushFont(fontlist[2]);
        ImGui::Text("Total Storage Used: ");

        if(hasRefreshed && totalAvail != 0) {
            const float percent = static_cast<float>(static_cast<long double>(totalUsed) / static_cast<long double>(
                totalAvail));
            int ipercent = static_cast<int>(percent * 100);
            ImGui::BufferingBar("##totalused", percent, ImVec2(200, 10), bg, fg);
            ImGui::SameLine();
            ImGui::Text("%d%% (%ld / %ld KB used)", ipercent, totalUsed.load(), totalAvail.load());
        }

        ImGui::Text("Relative repository storage usage");
        ImGui::PopFont();

        if(ImGui::BeginTable("##repousagetable", 1,
                             ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(300, 0))) {
            ImGui::TableNextRow();
            if(!hasRefreshed || exec) {
                ImGui::TableNextColumn();
                ImGui::Dummy(ImVec2(0, 25));
            }
            else
                for(const auto& pair : repousage) {
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(pair.first.c_str());
                    const float percent = static_cast<float>(static_cast<long double>(pair.second) / static_cast<long
                        double>(totalUsed));
                    ImGui::BufferingBar(("##buffering_"_st + pair.first).c_str(),
                                        percent, ImVec2(200, 10), bg, fg);
                    ImGui::SameLine();
                    ImGui::Text("%d%% (%ld / %ld bytes used)", static_cast<int>(percent * 100), pair.second,
                                totalUsed.load());
                }

            ImGui::EndTable();
        }

        if(!hasRefreshed) ImGui::EndDisabled();
        if(exec && !execDone) ImGui::BeginDisabled();

        if(ImGui::Button("Refresh")) {
            hasRefreshed = false;
            execDone = false;
            repousage.clear();
            success = 0;
            reponames = std::set<ST::string>(((RepoManage*)configScreens[ProgramStage::REPO_MANAGE])->getRepoNames());
            exec = new std::thread(&UsedStorage::refresh, this);
            ImGui::BeginDisabled();
        }

        if(exec && !execDone) {
            ImGui::EndDisabled();
            bakermaker::spinner();
        }

        if(execDone && exec) {
            exec->join();
            delete exec;
            exec = nullptr;
            if(success != 0) bakermaker::startErrorModal("Error getting storage usage!");
            else hasRefreshed = true;
        }
    }

    void UsedStorage::refresh() {
        ssh_session sess = nullptr;

        int rc = createSession(sess,
                               config["server"]["ip"].get<std::string>().c_str(),
                               config["server"]["user"].get<std::string>().c_str(),
                               config["server"]["keyfile"].get<std::string>().c_str(),
                               config["server"]["port"].get<int>());

        if(rc != SSH_OK) {
            success = -1;
            execDone = true;
            return;
        }

        totalAvail = getSizeOfSomething(sess, "/home/git/repositories"_st, FS_AVAIL);
        totalUsed = getSizeOfSomething(sess, "/home/git/repositories"_st, FS_USED);

        if(totalAvail < 0 || totalUsed < 0) {
            ssh_disconnect(sess);
            ssh_free(sess);
            success = -2;
            execDone = true;
            return;
        }

        for(const auto& str : reponames) {
            storage_t size = getSizeOfSomething(sess, ("/home/git/repositories/"_st + str + ".git"_st), DIR_USED);
            if(size < 0) {
                ssh_disconnect(sess);
                ssh_free(sess);
                success = -3;
                execDone = true;
                return;
            }

            repousage[str] = size;
        }

        ssh_disconnect(sess);
        ssh_free(sess);
        success = 0;
        execDone = true;
    }

    storage_t UsedStorage::getSizeOfSomething(ssh_session sess, const ST::string& path, int option) {
        ssh_channel ch = ssh_channel_new(sess);
        ssh_channel_open_session(ch);

        // ST::string cmd = usedOrTotal
        //                      ? "du --bytes --total "_st + path +
        //                      " | tail -n 1 | sed 's@^[^0-9]*\\([0-9]\\+\\).*@\1@'"_st
        //                      : ("sudo df --block-size=1 --output=avail " + path + " | tail -n 1"_st);

        ST::string cmd;

        switch(option) {
        case FS_USED:
            cmd = ("sudo df --block-size=1 --output=used " + path + " | tail -n 1"_st);
            break;

        case FS_AVAIL:
            cmd = ("sudo df --block-size=1 --output=avail " + path + " | tail -n 1"_st);
            break;

        case DIR_USED:
            cmd = "sudo du --bytes --total "_st + path + " | tail -n 1"_st;
            break;

        default:
            ssh_channel_send_eof(ch);
            ssh_channel_close(ch);
            ssh_channel_free(ch);
            return -1;
        }

        ssh_channel_request_exec(ch, cmd.c_str());

        char buffer[MSG_BUFFER_LENGTH];
        int nbytes = ssh_channel_read(ch, buffer, MSG_BUFFER_LENGTH - 1, 0);
        if(nbytes <= 0 || ssh_channel_get_exit_status(ch) != 0) return -1;
        buffer[nbytes - 1] = '\0';

        std::stringstream ss(buffer);
        storage_t ans;
        ss >> ans;

        ssh_channel_send_eof(ch);
        ssh_channel_close(ch);
        ssh_channel_free(ch);
        return ans / 1000;
    }
}
