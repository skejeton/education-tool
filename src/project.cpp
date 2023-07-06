#include "project.hpp"
#include <stdio.h>
#include "save.hpp"


static void save_load_project(Project *project, BinaryFormat *format)
{
    auto flashbacks_saver = TableSaver<FlashbacksDialog>::init(format, &project->flashbacks.dialogs);
    for (;flashbacks_saver.going(); flashbacks_saver.next()) {
        FlashbacksDialog *dialog = flashbacks_saver.save();

        format->pass_c_string((char**)&dialog->answer);
        format->pass_c_string((char**)&dialog->text);
        format->pass_value(&dialog->numeric);
        flashbacks_saver.pass_id(&dialog->prev);
        flashbacks_saver.pass_id(&dialog->next);
    }

    auto entities_saver = TableSaver<Entity>::init(format, &project->scene.entities);
    for (;entities_saver.going(); entities_saver.next())
    {
        Entity* entity = entities_saver.save();

        format->pass_value(&entity->position);
        format->pass_value(&entity->shape);
        format->pass_value(&entity->interaction_type);
        flashbacks_saver.pass_id(&entity->dialog_id);
    }
}


ProjectFile ProjectFile::init_from_path(const char *path)
{
    FILE *f = fopen(path, "rb");

    ProjectFile file = {};
    file.f = f;
    return file;
}


FileBuffer ProjectFile::read_project_data()
{
    return FileBuffer::read_whole_file(f);
}


void ProjectFile::write_project_data(FileBuffer buffer)
{
    buffer.write_whole_file(f);
}


void ProjectFile::deinit()
{
    fclose(f);
}


Project Project::load(ProjectFile f)
{
    FileBuffer buf = f.read_project_data();

    BinaryFormat format = BinaryFormat::begin_read(buf.data, buf.size);

    Project project = {};

    save_load_project(&project, &format);

    return project;
}


void Project::save(ProjectFile f)
{
    BinaryFormat format = BinaryFormat::begin_write();
    save_load_project(this, &format);

    f.write_project_data(format.leak_file_buffer());
}