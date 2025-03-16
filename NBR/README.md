# Nikola Binary Resource (NBR) 

A tool to convert any intermediary format to the *Nikola Binary Resource* format that is supported by the Nikola engine. The engine _only_ supports this format for loading resources. It will not accept a `.png` file to load an image, for example. For now, at least, the only way to use said `.png` file is to convert it using this tool and then feed it into the engine.  

Therefore, this tool's sole purpose is to convert any kind of _supported_ resource types into the various `nbr*` formats. It is fast, reliable, and fits perfectly into the engine's workflow. For now, the converter tool is a command line program that reads a `.nbrlist` file and generates the required `.nbr*` files. However, in the future (when I have time), a GUI program will be created, using _Nikola_'s own GUI system. 

## Supported Formats

Below you will find all the resource formats that can be converted into the `.nbr*` format.

*Images*: 
    - PNG 
    - JPEG 
    - TIFF 
    - BMP 
    - PSD 
    - TGA 
    - GIF 
    - HDR (although it isn't used in the engine as it is supposed to be, yet)
    - PIC 
    - PPM 
    - PGM

*Models*: 
    - OBJ 
    - FBX 
    - GLTF 

As for shaders, the only format the tool supports is the `.glsl` format. This file is actually just a combination of the two vertex and fragment shaders. Each type of shader is separated by the `#version` preprocessor. The file is read, scans through the contents, looks for the `#version` declarative and splits accordingly. 

## Usage 

This tool works in tandem with the `.nbrlist` file. The `.nbrlist` file is just a collection of paths, separated by sections to easily identify their types. In this directory, you can find the `list_guide.nbrlist` for a brief introduction to how the `.nbrlist` file is formatted. Once again, it is _extremely_ simple. 

The list of commands below can be used to describe certain user-level customizations. If any of them were to be omitted, the default values of each flag will be used. This is also shown below. 

All the sections below are actually preset variables inside the engine. For each section, 
an equivalent resource type is present. There can be more than one section associated 
with one resource type. But there can _never_ be an invalid section name. 


```bash
Usage: nbr [--parent-dir, -pd] [--bin-dir, -bd], [--resource-type, -rt] <path/to/list.nbrlist>
   --parent-dir    = The directory where all the input resources live (Default = current directory).
   --bin-dir       = The directory where all the output resources will be placed (Default = current directory).
   --resource-type = Specify a certain resource type to convert. If omitted, resources of all types will be converted ( Default = all).
   --help          = Show this help message.
```

The path to the `.nbrlist` file is a _required_ input. If any of the resource paths within the `.nbrlist` file are incorrect, the tool will throw an error, skip it, and move on to the next resource. Nothing will halt. Any wounded resources _will_ be left behind. The tool is not courageous. The tool is a machine.
