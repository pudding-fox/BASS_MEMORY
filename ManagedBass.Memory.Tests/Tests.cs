using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace ManagedBass.Memory.Tests
{
    [TestFixture]
    public class Tests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(Tests).Assembly.Location);

        [SetUp]
        public void SetUp()
        {
            Assert.IsTrue(Loader.Load("bass"));
            Assert.IsTrue(BassMemory.Load());
            Assert.IsTrue(Bass.Init());
        }

        [TearDown]
        public void TearDown()
        {
            BassMemory.Unload();
            Bass.Free();
        }

        [Explicit]
        [TestCase("01 Botanical Dimensions.m4a", 49062136, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 98124272, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 26975092, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 53950184, BassFlags.Float)]
        public void Test001(string fileName, long length, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = BassMemory.CreateStream(fileName, Flags: flags);
            if (sourceChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            Assert.AreEqual(length, Bass.ChannelGetLength(sourceChannel));
            Assert.IsTrue(Bass.ChannelPlay(sourceChannel));

            global::System.Threading.Thread.Sleep(10000);

            Assert.IsTrue(Bass.ChannelSetPosition(sourceChannel, Bass.ChannelGetLength(sourceChannel) - Bass.ChannelSeconds2Bytes(sourceChannel, 10)));

            while (Bass.ChannelIsActive(sourceChannel) == PlaybackState.Playing)
            {
                global::System.Threading.Thread.Sleep(1000);
            }

            Assert.AreEqual(length, Bass.ChannelGetPosition(sourceChannel));
        }

        [Explicit]
        [TestCase("01 Botanical Dimensions.m4a", 49062136, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 98124272, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 26975092, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 53950184, BassFlags.Float)]
        public void Test002(string fileName, long length, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = BassMemory.CreateStream(Bass.CreateStream(fileName, Flags: flags | BassFlags.Decode), Flags: flags);
            if (sourceChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            Assert.AreEqual(length, Bass.ChannelGetLength(sourceChannel));
            Assert.IsTrue(Bass.ChannelPlay(sourceChannel));

            global::System.Threading.Thread.Sleep(10000);

            Assert.IsTrue(Bass.ChannelSetPosition(sourceChannel, Bass.ChannelGetLength(sourceChannel) - Bass.ChannelSeconds2Bytes(sourceChannel, 10)));

            while (Bass.ChannelIsActive(sourceChannel) == PlaybackState.Playing)
            {
                global::System.Threading.Thread.Sleep(1000);
            }

            Assert.AreEqual(length, Bass.ChannelGetPosition(sourceChannel));
            Assert.IsTrue(Bass.StreamFree(sourceChannel));
        }

        [TestCase("01 Botanical Dimensions.m4a", 30547338, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 30547338, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 18741817, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 18741817, BassFlags.Float)]
        public void Test003(string fileName, long size, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannels = new int[10];
            for (var a = 0; a < 10; a++)
            {
                sourceChannels[a] = BassMemory.CreateStream(fileName, Flags: flags);
                if (sourceChannels[a] == 0)
                {
                    Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }
            }

            Assert.AreEqual(size, BassMemory.Usage());

            for (var a = 0; a < 10; a++)
            {
                Assert.IsTrue(Bass.StreamFree(sourceChannels[a]));
            }

            Assert.AreEqual(0, BassMemory.Usage());
        }

        [TestCase("01 Botanical Dimensions.m4a", 49062180, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 98124316, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 26975136, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 53950228, BassFlags.Float)]
        public void Test004(string fileName, long size, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = Bass.CreateStream(fileName, Flags: flags | BassFlags.Decode);
            if (sourceChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            var sourceChannels = new int[10];
            for (var a = 0; a < 10; a++)
            {
                sourceChannels[a] = BassMemory.CreateStream(sourceChannel, Flags: flags);
                if (sourceChannels[a] == 0)
                {
                    Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }
            }

            Assert.AreEqual(size, BassMemory.Usage());

            Assert.IsTrue(Bass.StreamFree(sourceChannel));
            for (var a = 0; a < 10; a++)
            {
                Assert.IsTrue(Bass.StreamFree(sourceChannels[a]));
            }

            Assert.AreEqual(0, BassMemory.Usage());
        }

        [TestCase("01 Botanical Dimensions.m4a", 49062136, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 98124272, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 26975092, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 53950184, BassFlags.Float)]
        public void Test005(string fileName, long length, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = BassMemory.CreateStream(fileName, Flags: flags | BassFlags.Decode);
            if (sourceChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            Assert.AreEqual(length, Bass.ChannelGetLength(sourceChannel));
            Assert.IsTrue(Bass.ChannelSetPosition(sourceChannel, length - 1));
            Assert.IsTrue(Bass.ChannelSetPosition(sourceChannel, length, PositionFlags.Bytes | PositionFlags.DecodeTo));
            Assert.AreEqual(length, Bass.ChannelGetPosition(sourceChannel));
            Assert.IsTrue(Bass.StreamFree(sourceChannel));
        }

        [TestCase("01 Botanical Dimensions.m4a", 49062136, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 98124272, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 26975092, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 53950184, BassFlags.Float)]
        public void Test006(string fileName, long length, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = BassMemory.CreateStream(Bass.CreateStream(fileName, Flags: flags | BassFlags.Decode), Flags: flags | BassFlags.Decode);
            if (sourceChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            Assert.AreEqual(length, Bass.ChannelGetLength(sourceChannel));
            Assert.IsTrue(Bass.ChannelSetPosition(sourceChannel, length - 1));
            Assert.IsTrue(Bass.ChannelSetPosition(sourceChannel, length, PositionFlags.Bytes | PositionFlags.DecodeTo));
            Assert.AreEqual(length, Bass.ChannelGetPosition(sourceChannel));
            Assert.IsTrue(Bass.StreamFree(sourceChannel));
        }

        [TestCase("01 Botanical Dimensions.m4a", 30547338, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 30547338, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 18741817, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 18741817, BassFlags.Float)]
        public void Test007(string fileName, long size, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            ProgressHandler.Attach(BassMemory.Progress);

            var sourceChannel = BassMemory.CreateStream(fileName, Flags: flags | BassFlags.Decode);
            if (sourceChannel == 0)
            {
                Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            Assert.IsTrue(Bass.StreamFree(sourceChannel));

            var events = default(IList<BassMemoryProgress>);
            Assert.IsTrue(ProgressHandler.Events.TryGetValue(fileName, out events));
            Assert.IsNotNull(events.SingleOrDefault(@event => @event.Type == BassMemoryProgressType.Begin));
            Assert.IsNotNull(events.SingleOrDefault(@event => @event.Position == size && @event.Length == size && @event.Type == BassMemoryProgressType.Update));
            Assert.IsNotNull(events.SingleOrDefault(@event => @event.Type == BassMemoryProgressType.End));

            ProgressHandler.Detach(BassMemory.Progress);
        }

        [TestCase("01 Botanical Dimensions.m4a", 49062180, BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", 98124316, BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", 26975136, BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", 53950228, BassFlags.Float)]
        public void Test008(string fileName, long size, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            ProgressHandler.Attach(BassMemory.Progress);

            var sourceChannel = Bass.CreateStream(fileName, Flags: flags | BassFlags.Decode);
            var memoryChannel = BassMemory.CreateStream(sourceChannel, Flags: flags | BassFlags.Decode);

            Assert.IsTrue(Bass.StreamFree(sourceChannel));
            Assert.IsTrue(Bass.StreamFree(memoryChannel));

            var events = default(IList<BassMemoryProgress>);
            Assert.IsTrue(ProgressHandler.Events.TryGetValue(string.Format("{0}.wav", Math.Abs(sourceChannel)), out events));
            Assert.IsNotNull(events.SingleOrDefault(@event => @event.Type == BassMemoryProgressType.Begin));
            Assert.IsNotNull(events.SingleOrDefault(@event => @event.Position == size && @event.Length == size && @event.Type == BassMemoryProgressType.Update));
            Assert.IsNotNull(events.SingleOrDefault(@event => @event.Type == BassMemoryProgressType.End));

            ProgressHandler.Detach(BassMemory.Progress);
        }

        [TestCase("01 Botanical Dimensions.m4a", BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", BassFlags.Float)]
        public void Test009(string fileName, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = Bass.CreateStream(fileName, Flags: flags | BassFlags.Decode);
            var memoryChannel = BassMemory.CreateStream(fileName, Flags: flags | BassFlags.Decode);

            var expected = ChannelReader.GetHashCode(sourceChannel);
            var actual = ChannelReader.GetHashCode(memoryChannel);

            Assert.AreEqual(expected, actual);

            Assert.IsTrue(Bass.StreamFree(sourceChannel));
            Assert.IsTrue(Bass.StreamFree(memoryChannel));
        }

        [TestCase("01 Botanical Dimensions.m4a", BassFlags.Default)]
        [TestCase("01 Botanical Dimensions.m4a", BassFlags.Float)]
        [TestCase("02 Outer Shpongolia.m4a", BassFlags.Default)]
        [TestCase("02 Outer Shpongolia.m4a", BassFlags.Float)]
        public void Test010(string fileName, BassFlags flags)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = Bass.CreateStream(fileName, Flags: flags | BassFlags.Decode);
            var memoryChannel = BassMemory.CreateStream(sourceChannel, Flags: flags | BassFlags.Decode);

            Bass.ChannelSetPosition(sourceChannel, 0);

            var expected = ChannelReader.GetHashCode(sourceChannel);
            var actual = ChannelReader.GetHashCode(memoryChannel);

            Assert.AreEqual(expected, actual);

            Assert.IsTrue(Bass.StreamFree(sourceChannel));
            Assert.IsTrue(Bass.StreamFree(memoryChannel));
        }

        [TestCase("01 Botanical Dimensions.m4a")]
        public void Test011(string fileName)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            BassMemory.Progress((ref BassMemoryProgress progress) =>
            {
                progress.Cancel = true;
            });

            var sourceChannel = BassMemory.CreateStream(fileName, Flags: BassFlags.Decode);
            Assert.AreEqual(0, sourceChannel);
        }

        [TestCase("01 Botanical Dimensions.m4a")]
        public void Test012(string fileName)
        {
            if (string.IsNullOrEmpty(Path.GetPathRoot(fileName)))
            {
                fileName = Path.Combine(Location, "Media", fileName);
            }

            var sourceChannel = Bass.CreateStream(fileName, Flags: BassFlags.Decode);

            BassMemory.Progress((ref BassMemoryProgress progress) =>
            {
                progress.Cancel = true;
            });

            var memoryChannel = BassMemory.CreateStream(sourceChannel, Flags: BassFlags.Decode);
            Assert.AreEqual(0, memoryChannel);
        }
    }
}
