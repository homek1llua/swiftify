#pragma once

// The system media transport controls (SMTC) are a winrt API. The toolchain
// that qt ships for its mingw kits carries an older mingw-w64 whose winrt
// headers stop short of windows.media: no ISystemMediaTransportControls, no
// display updater, no button-pressed args, and none of the uri and stream
// helpers updateMediaMetadata() reaches for. wrl/event.h, which holds
// Microsoft::WRL::Callback, is not part of mingw-w64 at all - the windows sdk
// owns it - so mingw builds need an equivalent written here.
//
// What follows declares the missing pieces in the shape the winrt headers
// give them, with the same interface ids, so the vtable layout matches the
// system exactly. Each block sits behind the guard widl writes for it, so on a
// toolchain that already ships the real declaration (msvc and the windows sdk,
// a newer mingw-w64) the block is skipped and the sdk's version is used.

#ifdef __MINGW32__

#include <eventtoken.h>
#include <inspectable.h>
#include <roapi.h>
#include <winstring.h>

#include <windows.foundation.collections.h>
#include <windows.storage.streams.h>

#include <wrl/client.h>
#include <wrl/internal.h>
#include <wrl/wrappers/corewrappers.h>

#include <atomic>
#include <utility>

// --- Microsoft::WRL::Wrappers::HString ---------------------------------------
//
// mingw's corewrappers.h declares HString as a friend of HStringReference and
// stops there, so the owning half - the one that makes its own string and hands
// the HSTRING to the system - is written here.

#if !defined(SWIFTIFY_HSTRING_WRAPPER_DEFINED)
#define SWIFTIFY_HSTRING_WRAPPER_DEFINED
namespace Microsoft {
namespace WRL {
namespace Wrappers {

class HString {
public:
    HString() throw() : hstr_(nullptr) {}

    HString(const wchar_t *str, unsigned int len) throw() : hstr_(nullptr)
    {
        const HRESULT result = ::WindowsCreateString(str, len, &hstr_);
        if (FAILED(result)) Details::RaiseException(result);
    }

    HString(const HString &other) throw() : hstr_(nullptr)
    {
        const HRESULT result = ::WindowsDuplicateString(other.hstr_, &hstr_);
        if (FAILED(result)) Details::RaiseException(result);
    }

    ~HString() throw() { ::WindowsDeleteString(hstr_); }

    HString &operator=(const HString &other) throw()
    {
        if (this != &other) {
            HSTRING copy = nullptr;
            const HRESULT result = ::WindowsDuplicateString(other.hstr_, &copy);
            if (FAILED(result)) Details::RaiseException(result);
            ::WindowsDeleteString(hstr_);
            hstr_ = copy;
        }
        return *this;
    }

    HSTRING Get() const throw() { return hstr_; }

    void Set(const wchar_t *str, unsigned int len) throw()
    {
        HSTRING copy = nullptr;
        const HRESULT result = ::WindowsCreateString(str, len, &copy);
        if (FAILED(result)) Details::RaiseException(result);
        ::WindowsDeleteString(hstr_);
        hstr_ = copy;
    }

private:
    HSTRING hstr_;
};

} // namespace Wrappers
} // namespace WRL
} // namespace Microsoft
#endif

// --- the runtime classes the interfaces below name ---------------------------
//
// winrt spells the sender and the event args as the runtime class rather than
// the interface, so both sides are declared before the interfaces that use
// them.

#if !defined(____x_ABI_CWindows_CMedia_CSystemMediaTransportControls_CLASS_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CSystemMediaTransportControls_CLASS_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

struct SystemMediaTransportControls;
struct SystemMediaTransportControlsButtonPressedEventArgs;
struct SystemMediaTransportControlsPropertyChangedEventArgs;

} // namespace Media
} // namespace Windows
} // namespace ABI
#endif

// --- windows.storage.streams: forward declarations ---------------------------
//
// the thumbnail accessors take a stream reference and the statics that make
// one take a stream, neither of which this toolchain declares

namespace ABI {
namespace Windows {
namespace Storage {
namespace Streams {

struct IRandomAccessStream;
struct IRandomAccessStreamReference;

} // namespace Streams
} // namespace Storage
} // namespace Windows
} // namespace ABI

// --- windows.foundation: the uri, forward declared ---------------------------

namespace ABI {
namespace Windows {
namespace Foundation {

struct IUriRuntimeClass;

} // namespace Foundation
} // namespace Windows
} // namespace ABI

// --- windows.media: the enums the transport controls take --------------------

#if !defined(____x_ABI_CWindows_CMedia_CMediaPlaybackStatus_ENUM_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CMediaPlaybackStatus_ENUM_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

enum MediaPlaybackStatus {
    MediaPlaybackStatus_Closed = 0,
    MediaPlaybackStatus_Changing = 1,
    MediaPlaybackStatus_Stopped = 2,
    MediaPlaybackStatus_Playing = 3,
    MediaPlaybackStatus_Paused = 4
};

} // namespace Media
} // namespace Windows
} // namespace ABI
#endif

#if !defined(____x_ABI_CWindows_CMedia_CMediaPlaybackType_ENUM_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CMediaPlaybackType_ENUM_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

enum MediaPlaybackType {
    MediaPlaybackType_Unknown = 0,
    MediaPlaybackType_Music = 1,
    MediaPlaybackType_Video = 2,
    MediaPlaybackType_Image = 3
};

} // namespace Media
} // namespace Windows
} // namespace ABI
#endif

#if !defined(____x_ABI_CWindows_CMedia_CSystemMediaTransportControlsButton_ENUM_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CSystemMediaTransportControlsButton_ENUM_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

enum SystemMediaTransportControlsButton {
    SystemMediaTransportControlsButton_Play = 0,
    SystemMediaTransportControlsButton_Pause = 1,
    SystemMediaTransportControlsButton_Stop = 2,
    SystemMediaTransportControlsButton_Record = 3,
    SystemMediaTransportControlsButton_FastForward = 4,
    SystemMediaTransportControlsButton_Rewind = 5,
    SystemMediaTransportControlsButton_Next = 6,
    SystemMediaTransportControlsButton_Previous = 7,
    SystemMediaTransportControlsButton_ChannelUp = 8,
    SystemMediaTransportControlsButton_ChannelDown = 9
};

} // namespace Media
} // namespace Windows
} // namespace ABI
#endif

// SoundLevel is not referenced by name here, but the transport controls'
// vtable carries get_SoundLevel in the middle of the run of is-enabled
// accessors, so the type has to exist for the layout to line up.
#if !defined(____x_ABI_CWindows_CMedia_CSoundLevel_ENUM_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CSoundLevel_ENUM_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

enum SoundLevel {
    SoundLevel_Muted = 0,
    SoundLevel_Low = 1,
    SoundLevel_Full = 2
};

} // namespace Media
} // namespace Windows
} // namespace ABI
#endif

// --- the interfaces themselves ----------------------------------------------

#if !defined(____x_ABI_CWindows_CMedia_CIMusicDisplayProperties_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CIMusicDisplayProperties_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

MIDL_INTERFACE("6bbf0c59-d0a0-4d26-92a0-f978e1d18e7b")
IMusicDisplayProperties : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_Title(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Title(HSTRING value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AlbumArtist(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AlbumArtist(HSTRING value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Artist(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Artist(HSTRING value) = 0;
};

} // namespace Media
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Media::IMusicDisplayProperties, 0x6bbf0c59, 0xd0a0, 0x4d26, 0x92, 0xa0, 0xf9, 0x78, 0xe1, 0xd1, 0x8e, 0x7b)
#endif

// AlbumTitle and TrackNumber arrived with the second generation of the
// interface, which winrt gives its own id and its own vtable rather than
// appending to the first: calling them through IMusicDisplayProperties would
// reach past the end of the object's vtable. Ask for this one instead.
#if !defined(____x_ABI_CWindows_CMedia_CIMusicDisplayProperties2_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CIMusicDisplayProperties2_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

MIDL_INTERFACE("00368462-97d3-44b9-b00f-008afcefaf18")
IMusicDisplayProperties2 : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_AlbumTitle(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AlbumTitle(HSTRING value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TrackNumber(UINT32 *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TrackNumber(UINT32 value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Genres(IInspectable **value) = 0;
};

} // namespace Media
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Media::IMusicDisplayProperties2, 0x00368462, 0x97d3, 0x44b9, 0xb0, 0x0f, 0x00, 0x8a, 0xfc, 0xef, 0xaf, 0x18)
#endif

#if !defined(____x_ABI_CWindows_CMedia_CISystemMediaTransportControlsDisplayUpdater_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CISystemMediaTransportControlsDisplayUpdater_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

MIDL_INTERFACE("8abbc53e-fa55-4ecf-ad8e-c984e5dd1550")
ISystemMediaTransportControlsDisplayUpdater : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_Type(MediaPlaybackType *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Type(MediaPlaybackType value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AppMediaId(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AppMediaId(HSTRING value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Thumbnail(ABI::Windows::Storage::Streams::IRandomAccessStreamReference **value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Thumbnail(ABI::Windows::Storage::Streams::IRandomAccessStreamReference *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MusicProperties(IMusicDisplayProperties **value) = 0;
    // the three getters below and CopyFromFileAsync sit in the vtable between
    // the music properties and ClearAll/Update; their own types are not
    // declared by the toolchain, and nothing here calls them, so the slots
    // are spelled out with the shapes winrt gives them
    virtual HRESULT STDMETHODCALLTYPE get_VideoProperties(IInspectable **value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ImageProperties(IInspectable **value) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyFromFileAsync(MediaPlaybackType type, IInspectable *source,
                                                        ABI::Windows::Foundation::IAsyncOperation<boolean> **operation) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearAll() = 0;
    virtual HRESULT STDMETHODCALLTYPE Update() = 0;
};

} // namespace Media
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Media::ISystemMediaTransportControlsDisplayUpdater, 0x8abbc53e, 0xfa55, 0x4ecf, 0xad, 0x8e, 0xc9, 0x84, 0xe5, 0xdd, 0x15, 0x50)
#endif

#if !defined(____x_ABI_CWindows_CMedia_CISystemMediaTransportControls_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CISystemMediaTransportControls_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

MIDL_INTERFACE("99fa3ff4-1742-42a6-902e-087d41f965ec")
ISystemMediaTransportControls : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_PlaybackStatus(MediaPlaybackStatus *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PlaybackStatus(MediaPlaybackStatus value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayUpdater(ISystemMediaTransportControlsDisplayUpdater **value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SoundLevel(SoundLevel *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsPlayEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsPlayEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsStopEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsStopEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsPauseEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsPauseEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRecordEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsRecordEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsFastForwardEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsFastForwardEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsRewindEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsRewindEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsPreviousEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsPreviousEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsNextEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsNextEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsChannelUpEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsChannelUpEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsChannelDownEnabled(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsChannelDownEnabled(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE add_ButtonPressed(
        ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls *, SystemMediaTransportControlsButtonPressedEventArgs *> *handler,
        EventRegistrationToken *token) = 0;
    virtual HRESULT STDMETHODCALLTYPE remove_ButtonPressed(EventRegistrationToken token) = 0;
    virtual HRESULT STDMETHODCALLTYPE add_PropertyChanged(
        ABI::Windows::Foundation::ITypedEventHandler<SystemMediaTransportControls *, SystemMediaTransportControlsPropertyChangedEventArgs *> *handler,
        EventRegistrationToken *token) = 0;
    virtual HRESULT STDMETHODCALLTYPE remove_PropertyChanged(EventRegistrationToken token) = 0;
};

} // namespace Media
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Media::ISystemMediaTransportControls, 0x99fa3ff4, 0x1742, 0x42a6, 0x90, 0x2e, 0x08, 0x7d, 0x41, 0xf9, 0x65, 0xec)
#endif

#if !defined(____x_ABI_CWindows_CMedia_CISystemMediaTransportControlsButtonPressedEventArgs_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CMedia_CISystemMediaTransportControlsButtonPressedEventArgs_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Media {

MIDL_INTERFACE("b7f47116-a56f-4dc8-9e11-92031f4a87c2")
ISystemMediaTransportControlsButtonPressedEventArgs : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_Button(SystemMediaTransportControlsButton *value) = 0;
};

} // namespace Media
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Media::ISystemMediaTransportControlsButtonPressedEventArgs, 0xb7f47116, 0xa56f, 0x4dc8, 0x9e, 0x11, 0x92, 0x03, 0x1f, 0x4a, 0x87, 0xc2)
#endif

#if !defined(RUNTIMECLASS_Windows_Media_SystemMediaTransportControls_DEFINED)
#define RUNTIMECLASS_Windows_Media_SystemMediaTransportControls_DEFINED
extern const DECLSPEC_SELECTANY WCHAR RuntimeClass_Windows_Media_SystemMediaTransportControls[] =
    L"Windows.Media.SystemMediaTransportControls";
#endif

// --- the button-pressed handler ---------------------------------------------
//
// the transport controls' event is a typed event handler, and winrt gives each
// instantiation its own id. The toolchain declares the template but not this
// specialisation, so the system's id for it is declared here: the object
// handed to add_ButtonPressed() is asked for exactly this id.

#if !defined(SWIFTIFY_SMT_BUTTON_PRESSED_HANDLER_DEFINED)
#define SWIFTIFY_SMT_BUTTON_PRESSED_HANDLER_DEFINED
namespace ABI {
namespace Windows {
namespace Foundation {

template <>
MIDL_INTERFACE("0557e996-7b23-5bae-aa81-ea0d671143a4")
ITypedEventHandler<ABI::Windows::Media::SystemMediaTransportControls *,
                   ABI::Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs *>
    : ITypedEventHandler_impl<ABI::Windows::Media::SystemMediaTransportControls *,
                              ABI::Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs *>
{
};

} // namespace Foundation
} // namespace Windows
} // namespace ABI

// the id is handed to __CRT_UUID_DECL through an alias, as a preprocessor
// splits the template's arguments at the comma
namespace swiftify {
namespace detail {

using SmtcButtonPressedHandler = ABI::Windows::Foundation::ITypedEventHandler<
    ABI::Windows::Media::SystemMediaTransportControls *,
    ABI::Windows::Media::SystemMediaTransportControlsButtonPressedEventArgs *>;

} // namespace detail
} // namespace swiftify

__CRT_UUID_DECL(swiftify::detail::SmtcButtonPressedHandler, 0x0557e996, 0x7b23, 0x5bae, 0xaa, 0x81, 0xea, 0x0d, 0x67, 0x11, 0x43, 0xa4)
#endif

// --- windows.foundation: the uri --------------------------------------------

#if !defined(____x_ABI_CWindows_CFoundation_CIUriRuntimeClassFactory_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CFoundation_CIUriRuntimeClassFactory_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Foundation {

MIDL_INTERFACE("44a9796f-723e-4fdf-a218-033e75b0c084")
IUriRuntimeClassFactory : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE CreateUri(HSTRING uri, IUriRuntimeClass **instance) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateWithRelativeUri(HSTRING base_uri, HSTRING relative_uri,
                                                            IUriRuntimeClass **instance) = 0;
};

} // namespace Foundation
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Foundation::IUriRuntimeClassFactory, 0x44a9796f, 0x723e, 0x4fdf, 0xa2, 0x18, 0x03, 0x3e, 0x75, 0xb0, 0xc0, 0x84)
#endif

#if !defined(____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CFoundation_CIUriRuntimeClass_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Foundation {

MIDL_INTERFACE("9e365e57-48b2-4160-956f-c7385120bbfc")
IUriRuntimeClass : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE get_AbsoluteUri(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayUri(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Domain(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Extension(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Fragment(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Host(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Password(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Query(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Scheme(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserInfo(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Port(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsAbsoluteUri(boolean *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserDisplayName(HSTRING *value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OriginalString(HSTRING *value) = 0;
};

} // namespace Foundation
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Foundation::IUriRuntimeClass, 0x9e365e57, 0x48b2, 0x4160, 0x95, 0x6f, 0xc7, 0x38, 0x51, 0x20, 0xbb, 0xfc)
#endif

#if !defined(RUNTIMECLASS_Windows_Foundation_Uri_DEFINED)
#define RUNTIMECLASS_Windows_Foundation_Uri_DEFINED
extern const DECLSPEC_SELECTANY WCHAR RuntimeClass_Windows_Foundation_Uri[] = L"Windows.Foundation.Uri";
#endif

// --- windows.storage.streams: the thumbnail source --------------------------

#if !defined(____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStreamReferenceStatics_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStreamReferenceStatics_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Storage {
namespace Streams {

MIDL_INTERFACE("857309dc-3fbf-4e7d-986f-ef3b1a07a964")
IRandomAccessStreamReferenceStatics : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE CreateFromFile(IInspectable *file,
                                                    IRandomAccessStreamReference **stream_reference) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFromUri(ABI::Windows::Foundation::IUriRuntimeClass *uri,
                                                    IRandomAccessStreamReference **stream_reference) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFromStream(IRandomAccessStream *stream,
                                                       IRandomAccessStreamReference **stream_reference) = 0;
};

} // namespace Streams
} // namespace Storage
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Storage::Streams::IRandomAccessStreamReferenceStatics, 0x857309dc, 0x3fbf, 0x4e7d, 0x98, 0x6f, 0xef, 0x3b, 0x1a, 0x07, 0xa9, 0x64)
#endif

#if !defined(____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStreamReference_INTERFACE_DEFINED__)
#define ____x_ABI_CWindows_CStorage_CStreams_CIRandomAccessStreamReference_INTERFACE_DEFINED__
namespace ABI {
namespace Windows {
namespace Storage {
namespace Streams {

MIDL_INTERFACE("33ee3134-1dd6-4e3a-8067-d1c162e8642b")
IRandomAccessStreamReference : public IInspectable
{
    virtual HRESULT STDMETHODCALLTYPE OpenReadAsync(
        ABI::Windows::Foundation::IAsyncOperation<IRandomAccessStreamWithContentType *> **operation) = 0;
};

} // namespace Streams
} // namespace Storage
} // namespace Windows
} // namespace ABI
__CRT_UUID_DECL(ABI::Windows::Storage::Streams::IRandomAccessStreamReference, 0x33ee3134, 0x1dd6, 0x4e3a, 0x80, 0x67, 0xd1, 0xc1, 0x62, 0xe8, 0x64, 0x2b)
#endif

#if !defined(RUNTIMECLASS_Windows_Storage_Streams_RandomAccessStreamReference_DEFINED)
#define RUNTIMECLASS_Windows_Storage_Streams_RandomAccessStreamReference_DEFINED
extern const DECLSPEC_SELECTANY WCHAR RuntimeClass_Windows_Storage_Streams_RandomAccessStreamReference[] =
    L"Windows.Storage.Streams.RandomAccessStreamReference";
#endif

// --- Microsoft::WRL::Callback ------------------------------------------------
//
// wrl/event.h never reaches mingw-w64, so the one use of it here - wrapping a
// lambda as a winrt event handler - is written out. The handler the system
// takes is a com object with the iid of the typed event handler it is asked
// for, so the object below is a plain IUnknown with the interface's vtable in
// front of it and the lambda in the slot.

#if !__has_include(<wrl/event.h>)

namespace Microsoft {
namespace WRL {
namespace Details {

// winrt spells an event handler's arguments as the runtime class, but hands
// over the default interface, which is what a handler written against the
// interface expects. The two are the same pointer either way, so the handler's
// own parameter types are read off its call operator.
template <typename THandler>
struct HandlerArguments : HandlerArguments<decltype(&THandler::operator())> {};

template <typename TClass, typename TResult, typename TSender, typename TArgs>
struct HandlerArguments<TResult (TClass::*)(TSender, TArgs) const> {
    using Sender = TSender;
    using Args = TArgs;
};

template <typename TClass, typename TResult, typename TSender, typename TArgs>
struct HandlerArguments<TResult (TClass::*)(TSender, TArgs)> {
    using Sender = TSender;
    using Args = TArgs;
};

template <typename TDelegate, typename THandler>
class ComEventHandler final : public TDelegate
{
public:
    explicit ComEventHandler(THandler handler) : m_handler(std::move(handler)) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **object) override
    {
        if (!object) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(TDelegate)) {
            *object = static_cast<TDelegate *>(this);
            AddRef();
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override { return m_references.fetch_add(1) + 1; }

    ULONG STDMETHODCALLTYPE Release() override
    {
        if (m_references.fetch_sub(1) == 1) delete this;
        return 1;
    }

    HRESULT STDMETHODCALLTYPE Invoke(typename TDelegate::TSender_complex sender,
                                     typename TDelegate::TArgs_complex args) override
    {
        return m_handler(reinterpret_cast<typename HandlerArguments<THandler>::Sender>(sender),
                         reinterpret_cast<typename HandlerArguments<THandler>::Args>(args));
    }

private:
    std::atomic<ULONG> m_references {1};
    THandler m_handler;
};

// WRL's Callback takes the interface, and hands back a ComPtr over it. Only
// the single-interface form is needed here, so that is what is offered.
template <typename TDelegate, typename THandler>
ComPtr<TDelegate> Callback(const THandler &handler)
{
    return ComPtr<TDelegate>(new ComEventHandler<TDelegate, THandler>(handler));
}

} // namespace Details

using Details::Callback;

} // namespace WRL
} // namespace Microsoft

#endif // !__has_include(<wrl/event.h>)

#endif // __MINGW32__