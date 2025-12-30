#pragma once

class Repository;

void handle_upload_pack(Repository& repo);
void handle_receive_pack(Repository& repo);

void handle_ls_refs(Repository& repo);
