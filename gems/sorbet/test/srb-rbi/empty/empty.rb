# frozen_string_literal: true
# typed: ignore

require 'minitest/autorun'
require 'minitest/spec'
require 'mocha/minitest'

require 'tmpdir'
require 'bundler'

class Sorbet; end
module Sorbet::Private; end
module Sorbet::Private::Test; end
class Sorbet::Private::Test::Empty < MiniTest::Spec
  it 'works on an empty example' do
    Dir.mktmpdir do |dir|
      Dir.chdir(dir)

      olddir = __dir__
      Dir.foreach(olddir + '/empty/') do |item|
        FileUtils.cp_r(olddir + '/empty/' + item, dir)
      end

      out = ''

      Bundler.with_clean_env do
        out = IO.popen(
          {'SRB_YES' => '1'},
          ['bundle', 'exec', olddir + '/../../../bin/srb-rbi'],
          'r+',
          &:read
        )
      end

      out = out.gsub(/with \d+ modules and \d+ aliases/, 'with %d modules and %d aliases')

      if ENV['RECORD']
        File.write(olddir + '/empty.out', out)
      end
      assert_equal(File.read(olddir + '/empty.out'), out)
      assert_equal(true, $?.success?)

      assert_dirs_equal(olddir + '/sorbet', dir + '/sorbet')
    end
  end

  def assert_dirs_equal(expdir, dir)
    seen = []
    Dir.foreach(dir) do |item|
      seen << item
      next if (item == ".") || (item == "..")
      expfile = expdir + '/' + item
      file = dir + '/' + item
      if File.directory?(file)
        if ENV['RECORD'] && !Dir.exist?(expfile)
          Dir.mkdir(expfile)
        end
        assert_dirs_equal(expfile, file)
      else
        if ENV['RECORD']
          File.write(expfile, read_file(file))
        end
        assert_equal(read_file(expfile), read_file(file), "#{expfile} != #{file}\n#{read_file(expfile)}\n#{read_file(file)}")
      end
    end
    Dir.foreach(expdir) do |item|
      next if seen.include?(item)
      expfile = expdir + '/' + item
      flunk("#{expfile} is missing in output")
    end
  end

  def read_file(fname)
    File.read(fname)
      .gsub(/sorbet-\d+[.]\d+[.]\d+/, 'sorbet-%d.%d.%d')
  end
end